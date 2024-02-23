// fzn-pbs.cpp

// Includes
#include <iostream>
#include <fstream>
#include <gecode/flatzinc.hh>
#include <gecode/flatzinc/registry.hh>
#include <gecode/flatzinc/plugin.hh>
#include <gecode/flatzinc/branch.hh>
#include <gecode/search.hh>
#include <gecode/flatzinc/fzn-pbs.hh>
#include <gecode/flatzinc/search-engine-base.hh>

#include <array>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <unordered_set>

using namespace std;
using namespace Gecode;
using namespace Gecode::FlatZinc;

FznPbs::FznPbs(FlatZinc::FlatZincSpace* fg, const int assets) : fg(fg), assets(assets), asset_spaces(assets), solution_found(false), assets_solve_times(assets), assets_status_stats(assets), assets_num_propagators(assets), best_sol(nullptr), finished_asset(-1){
    execution_done_wait_started = false;
    running_threads = 2;
    // Initialize global_objective given method:
    method = fg->method();
}

FznPbs::~FznPbs() {
    for (int i = 0; i < assets; i++) {
        delete asset_spaces[i];
    }
}

void FznPbs::thread_done() {
    if (running_threads.fetch_sub(1) == 1) {
        execution_done_event.signal();
    }
}

void FznPbs::await_runners_completed() {
    execution_done_event.wait();
}

// Print the statistics of the search.
void FznPbs::solutionStatistics(FlatZincSpace* fg, BaseEngine* se, Support::Timer t_solve, StatusStatistics sstat, int n_p, std::ostream& out, Support::Timer& t_total) {
    Gecode::Search::Statistics stat = se->statistics();
    double totalTime = (t_total.stop() / 1000.0);
    double solveTime = (t_solve.stop() / 1000.0);
    double initTime = totalTime - solveTime;
    out << std::endl
        << "%%%mzn-stat: initTime=" << initTime
        << std::endl;
    out << "%%%mzn-stat: solveTime=" << solveTime
        << std::endl;
    // out << "%%%mzn-stat: solutions="
    //     << std::abs(fopt.solutions - findSol) << std::endl
    out << "%%%mzn-stat: variables="
        << (fg->getintVarCount() + fg->getboolVarCount() + fg->getsetVarCount()) << std::endl
        << "%%%mzn-stat: propagators=" << n_p << std::endl
        << "%%%mzn-stat: propagations=" << sstat.propagate+stat.propagate << std::endl
        << "%%%mzn-stat: nodes=" << stat.node << std::endl
        << "%%%mzn-stat: failures=" << stat.fail << std::endl
        << "%%%mzn-stat: restarts=" << stat.restart << std::endl
        << "%%%mzn-stat: peakDepth=" << stat.depth << std::endl
        << "%%%mzn-stat-end" << std::endl
        << std::endl;
}

// Set up the search options for the different searches.
Search::Options FznPbs::setupAssetSearchOptions(FlatZincSpace* fg, FlatZincOptions& fopt, FlatZinc::Printer& p, unsigned int c_d, unsigned int a_d, double threads, bool use_rbs = false, unsigned int restart_type = 1, double restart_base = 1.5, unsigned int restart_scale = 250){
    Search::Options search_options;
    // Inherit from CombinedStop som kollar solution_found också.
    search_options.stop = Driver::CombinedStop::create(fopt.node(), fopt.fail(), fopt.time(), fopt.restart_limit(), true);
    search_options.c_d = c_d;
    search_options.a_d = a_d;

    #ifdef GECODE_HAS_CPPROFILER
        if (fopt.profiler_port()) {
            FlatZincGetInfo* getInfo = nullptr;

            if (fopt.profiler_info()) getInfo = new FlatZincGetInfo(p);
            
            search_options.tracer = new CPProfilerSearchTracer(fopt.profiler_id(), fopt.name(), fopt.profiler_port(), getInfo);
        }

    #endif

    #ifdef GECODE_HAS_FLOAT_VARS
        fg->step = fopt.step();
    #endif

    search_options.threads = threads;
    search_options.nogoods_limit = fopt.nogoods() ? fopt.nogoods_limit() : 0;

    // If not RBS but asset is to use it:
    if (fopt.restart() == RM_NONE && use_rbs){
        fopt.restart(RestartMode(restart_type));
        fopt.restart_base(restart_base);
        fopt.restart_scale(restart_scale);
        
        // Create the branchers. (Needs to be here due to non-rbs options and assets that may utilize rbs.)
        fg->createBranchers(p, fg->solveAnnotations(), fopt, false, std::cerr);
        fg->shrinkArrays(p);

        search_options.cutoff  = new Search::CutoffAppend(new Search::CutoffConstant(0), 1, Driver::createCutoff(fopt));

        // Set fopt back to RM_NONE.
        fopt.restart(RM_NONE);
    }
    // If default option is RBS:
    else if (fopt.restart() != RM_NONE && use_rbs){
        int restart_type = fopt.restart();
        fopt.restart(RM_NONE);
        fg->createBranchers(p, fg->solveAnnotations(), fopt, false, std::cerr);
        fg->shrinkArrays(p);
        fopt.restart(RestartMode(restart_type));

        search_options.cutoff = new Search::CutoffAppend(new Search::CutoffConstant(0), 1, Driver::createCutoff(fopt));
    }
    // If not RBS:
    else{
        fg->createBranchers(p, fg->solveAnnotations(), fopt, false, std::cerr);
        fg->shrinkArrays(p);

        search_options.cutoff = new Search::CutoffAppend(new Search::CutoffConstant(0), 1, Driver::createCutoff(fopt));
    }

    if (fopt.interrupt()) Driver::CombinedStop::installCtrlHandler(true);

    return search_options;
}

void FznPbs::setupPortfolioAssets(int asset, FlatZinc::Printer& p, FlatZincOptions& fopt) {
    // Set up the portfolio assets.
    asset_spaces[asset] = static_cast<FlatZinc::FlatZincSpace*>(fg->clone());

    // Set controller as shared pointer to every flatzincspace
    // Copy iv,bv,sv_introduced vector from fg, as it does not follow the clone.
    asset_spaces[asset]->iv_introduced = std::vector<bool>(fg->iv_introduced);
    asset_spaces[asset]->bv_introduced = std::vector<bool>(fg->bv_introduced);
    asset_spaces[asset]->sv_introduced = std::vector<bool>(fg->sv_introduced);

    Search::Options search_options;
    switch (asset)
    // Depending on what asset it is, set up the engine for the asset and the specific brancher.
    {
    // Base asset, is the same as the one given by the user.
    case 0:
        search_options = setupAssetSearchOptions(asset_spaces[asset], fopt, p, fopt.c_d(), fopt.a_d(), fopt.threads(), static_cast<bool>(fopt.restart()));
        asset_engines.push_back(new BABEngine(asset_spaces[asset], search_options));
        break;
    // Second asset (LNS for now)
    case 1:
        search_options = setupAssetSearchOptions(asset_spaces[asset], fopt, p, fopt.c_d(), fopt.a_d(), fopt.threads(), true, 1, 1.5, 250);
        asset_engines.push_back(new RBSEngine(asset_spaces[asset], search_options));
        break;
    default:
        break;
    }
}

// The controller that creates the workers and controls the searches.
void FznPbs::controller(std::ostream& out, FlatZinc::Printer& p, FlatZincOptions& fopt, Support::Timer& t_total) {
    // Make search space clone-able by calling status on it. If it fails, then the model is unsatisfiable.
    if (fg->status() == SS_FAILED) {
        out << "=====UNSATISFIABLE=====" << std::endl;
        return;
    }

    fg->status();

    // Set up each asset for the portfolio.
    for (int asset = 0; asset < assets; asset++) {
        setupPortfolioAssets(asset, p, fopt);
    }

    for (int asset = 0; asset < assets; asset++) {
        Gecode::Support::Thread::run(new SearchExecutor(*this, asset, out, fopt, p));
    }
    await_runners_completed();

    // Print the best or final solution:
    FlatZincSpace* sol = asset_spaces[finished_asset];
    BaseEngine* se = asset_engines[finished_asset];
    if (sol) {
        sol->print(out, p);
        out << "----------" << std::endl;
    }

    if (!se->stopped()) {
        if (sol) {
        out << "==========" << std::endl;
        } else {
        out << "=====UNSATISFIABLE=====" << std::endl;
        }
    }
    else if (!sol) {
        out << "=====UNKNOWN=====" << std::endl;
    }

    // If to print Statistics:
    if (fopt.mode() == SM_STAT) {
        solutionStatistics(sol, se, assets_solve_times[finished_asset], assets_status_stats[finished_asset], assets_num_propagators[finished_asset], out, t_total);
    }

    delete sol;
}


// ########################################################################
//                         SearchExecutor below.
// ########################################################################

SearchExecutor::SearchExecutor(FznPbs& control, int asset, std::ostream& out, FlatZincOptions& fopt, FlatZinc::Printer& p)
    : control(control), fg(control.asset_spaces[asset]), se(control.asset_engines[asset]), out(out), fopt(fopt), p(p), asset(asset) {}

// FlatZincSpace::constrain(const Space& s) { för att uppdatera bounds
void SearchExecutor::run(void){
    // Start the search timer.
    control.assets_solve_times[asset].start();
    StatusStatistics sstat;
    if (fg->status(sstat) != SS_FAILED) {
        control.assets_num_propagators[asset] = PropagatorGroup::all.size(*fg);
        control.assets_status_stats[asset] = sstat;
    }
    else{
        out << "=====UNSATISFIABLE=====" << std::endl;
        control.thread_done();
        return;
    }


    // Run the search
    FlatZincSpace* sol = nullptr;
    while (FlatZincSpace* next_sol = se->next()) {
        // delete sol;
        sol = next_sol;

        // If one asset finished, stop looking for more solutions.
        if (control.solution_found.load()) {
            break;
        }


        int optVar = sol->optVar();
        while(true){
            FlatZincSpace* control_best_sol = control.best_sol.load();

            if (control_best_sol == nullptr){
                    control.best_space_mutex.lock();
                    // Critical Section
                    FlatZincSpace* expected = nullptr;
                    bool success = control.best_sol.compare_exchange_strong(expected, sol);
                    control.best_space_mutex.unlock();

                    if (success){
                        break;
                    }
            }
            else{
                if (control.method == 2){
                    if (control_best_sol->iv[optVar].val() < sol->iv[optVar].val()){
                        control.best_space_mutex.lock();
                        // Critical Section
                        FlatZincSpace* expected = control.best_sol.load();
                        if (expected->iv[optVar].val() < sol->iv[optVar].val()){
                            bool success = control.best_sol.compare_exchange_strong(expected, sol);
                            assert(success);
                        }
                        // Memory leak from expected when swapped.
                        control.best_space_mutex.unlock();
                    }
                }
                else{
                    if (control_best_sol->iv[optVar].val() > sol->iv[optVar].val()){
                        control.best_space_mutex.lock();
                        // Critical Section
                        FlatZincSpace* expected = control.best_sol.load();
                        if (expected->iv[optVar].val() > sol->iv[optVar].val()){
                            bool success = control.best_sol.compare_exchange_strong(expected, sol);
                            assert(success);
                        }
                        control.best_space_mutex.unlock();
                    }
                }
                break;
            }
        }
    }
    control.assets_solve_times[asset].stop();

    // If optimal solution was found, print it out.
    if (!control.solution_found.exchange(true)){
        control.finished_asset = asset;
        FlatZincSpace* old_space = control.asset_spaces[asset];
        control.asset_spaces[asset] = static_cast<FlatZincSpace*>(sol->clone());
        delete old_space;
    }

    // delete sol;
    control.thread_done();
}