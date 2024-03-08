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

FznPbs::FznPbs(FlatZinc::FlatZincSpace* fg, const int assets, FlatZinc::Printer& p)
    : fg(fg), assets(assets), 
      asset_spaces(assets), 
      asset_engines(assets), 
      asset_best_solutions(assets), 
      p(p), 
      search_finished(false), 
      assets_solve_times(assets), 
      assets_status_stats(assets), 
      assets_num_propagators(assets), 
      best_sol(nullptr), 
      finished_asset(-1)
    {
    execution_done_wait_started = false;
    running_threads = assets;
    // Initialize global_objective given method:
    method = fg->method();
}

FznPbs::~FznPbs() {
    for (int i = 0; i < assets; i++) {
        delete asset_best_solutions[i];
        delete asset_engines[i];
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
void FznPbs::solutionStatistics(FlatZincSpace* fg, BaseEngine* se, Support::Timer t_solve, StatusStatistics sstat, int n_p, std::ostream& out, Support::Timer& t_total, bool allAssetStat = false) {
    Gecode::Search::Statistics stat = se->statistics();
    double totalTime = (t_total.stop() / 1000.0);
    double solveTime = (t_solve.stop() / 1000.0);
    double initTime = totalTime - solveTime;

    if (allAssetStat){
        out << std::endl
            << "%%%mzn-stat: initTime=" << initTime
            << std::endl;
        out << "%%%mzn-stat: solveTime=" << solveTime
            << std::endl;
        out << "%%%mzn-stat: finished asset="
            << finished_asset << std::endl;
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

        for (int asset = 0; asset < assets; asset++){
            if (asset == finished_asset){
                continue;
            }
            stat = asset_engines[asset]->statistics();
            n_p = assets_num_propagators[asset];
            out << std::endl
                << "%%%mzn-stat: initTime=" << initTime
                << std::endl;
            out << "%%%mzn-stat: solveTime=" << solveTime
                << std::endl;
            out << "%%%mzn-stat: unfinished asset="
                << asset << std::endl;
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
    }
    else{
        out << std::endl
            << "%%%mzn-stat: initTime=" << initTime
            << std::endl;
        out << "%%%mzn-stat: solveTime=" << solveTime
            << std::endl;
        out << "%%%mzn-stat: finished asset="
            << finished_asset << std::endl;
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
}

// Set up the search options for the different searches.
Search::Options FznPbs::setupAssetSearchOptions(FlatZincSpace* fg, FlatZincOptions& fopt, FlatZinc::Printer& p, unsigned int c_d, unsigned int a_d, double threads, string brancher, int asset, bool use_rbs = false, unsigned int restart_type = 1, double restart_base = 1.5, unsigned int restart_scale = 250){
    Search::Options search_options;
    search_options.stop = Driver::PBSCombinedStop::create(fopt.node(), fopt.fail(), fopt.time(), fopt.restart_limit(), true, search_finished);
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

    AST::Array* branch_strat = fg->solveAnnotations();
    if (brancher != ""){
        if (AST::Atom* s = dynamic_cast<AST::Atom*>(branch_strat)){
            s->id = brancher;
            branch_strat = dynamic_cast<AST::Array*>(s);
        }
    }


    // If not RBS but asset is to use it:
    if (fopt.restart() == RM_NONE && use_rbs){
        fopt.restart(RestartMode(restart_type));
        fopt.restart_base(restart_base);
        fopt.restart_scale(restart_scale);
        
        // Create the branchers. (Needs to be here due to non-rbs options and assets that may utilize rbs.)
        fg->createBranchers(p, branch_strat, fopt, false, std::cerr);
        if (asset == assets){
            fg->shrinkArrays(p);
        }
       

        search_options.cutoff  = new Search::CutoffAppend(new Search::CutoffConstant(0), 1, Driver::createCutoff(fopt));

        // Set fopt back to RM_NONE.
        fopt.restart(RM_NONE);
    }
    // If default option is RBS but asset is not to use it:
    else if (fopt.restart() != RM_NONE && !use_rbs){
        int restart_type = fopt.restart();
        fopt.restart(RM_NONE);

        fg->createBranchers(p, branch_strat, fopt, false, std::cerr);
        if (asset == assets){
            fg->shrinkArrays(p);
        }
        
        fopt.restart(RestartMode(restart_type));

        search_options.cutoff = new Search::CutoffAppend(new Search::CutoffConstant(0), 1, Driver::createCutoff(fopt));
    }
    // Else default:
    else{
        fg->createBranchers(p, branch_strat, fopt, false, std::cerr);
        if (asset == assets){
            fg->shrinkArrays(p);
        }

        search_options.cutoff = new Search::CutoffAppend(new Search::CutoffConstant(0), 1, Driver::createCutoff(fopt));
    }

    if (fopt.interrupt()) Driver::CombinedStop::installCtrlHandler(true);

    return search_options;
}

void FznPbs::setupPortfolioAssets(int asset, FlatZinc::Printer& p, FlatZincOptions& fopt) {
    // Set up the portfolio assets.
    asset_spaces[asset] = static_cast<FlatZinc::FlatZincSpace*>(fg->clone());
    asset_spaces[asset]->pbs_current_best_sol = &best_sol;

    // Copy iv,bv,sv_introduced vector from fg, as it does not follow the clone.
    asset_spaces[asset]->iv_introduced = fg->iv_introduced;
    asset_spaces[asset]->bv_introduced = fg->bv_introduced;
    asset_spaces[asset]->sv_introduced = fg->sv_introduced;

    Search::Options search_options;
    switch (Asset(asset))
    // Depending on what asset it is, set up the engine for the asset and the specific brancher.
    {
    // Base asset, is the same as the one given by the user.
    case USER:
        search_options = setupAssetSearchOptions(asset_spaces[asset], fopt, p, fopt.c_d(), fopt.a_d(), fopt.threads(), "", asset, false);
        asset_engines[asset] = new BABEngine(asset_spaces[asset], search_options);
        break;
    // Second asset (LNS for now)
    case LNS_USER:
        search_options = setupAssetSearchOptions(asset_spaces[asset], fopt, p, fopt.c_d(), fopt.a_d(), fopt.threads(), "", asset, true, 1, 1.5, 250);
        asset_engines[asset] = new RBSEngine(asset_spaces[asset], search_options);
        break;
    // Third asset opposite branchin (for now)
    case USER_OPPOSITE:
        search_options = setupAssetSearchOptions(asset_spaces[asset], fopt, p, fopt.c_d(), fopt.a_d(), fopt.threads(), "largest", asset, false);
        asset_engines[asset] = new BABEngine(asset_spaces[asset], search_options);
        break;

    default:
        break;
    }
}

// The controller that creates the workers and controls the searches.
void FznPbs::controller(std::ostream& out, FlatZincOptions& fopt, Support::Timer& t_total) {
    // Make search space clone-able by calling status on it. If it fails, then the model is unsatisfiable.
    if (fg->status() == SS_FAILED) {
        out << "=====UNSATISFIABLE=====" << std::endl;
        return;
    }

    for (int asset = 0; asset < assets; asset++) {
        setupPortfolioAssets(asset, p, fopt);
        Gecode::Support::Thread::run(new SearchExecutor(*this, asset, out, fopt, p));
    }
    await_runners_completed();

    // Print the best or final solution:
    FlatZincSpace* sol = best_sol.load();
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
        solutionStatistics(sol, se, assets_solve_times[finished_asset], assets_status_stats[finished_asset], assets_num_propagators[finished_asset], out, t_total, fopt.fullStatistics());
    }
}


// ########################################################################
//                         SearchExecutor below.
// ########################################################################

SearchExecutor::SearchExecutor(FznPbs& control, int asset, std::ostream& out, FlatZincOptions& fopt, FlatZinc::Printer& p)
    : control(control), fg(control.asset_spaces[asset]), se(control.asset_engines[asset]), out(out), fopt(fopt), p(p), asset(asset) {}

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

    bool printAll = fopt.allSolutions();

    // Run the search
    FlatZincSpace* sol = nullptr;
    bool solWasBestSol = false;
    while (FlatZincSpace* next_sol = se->next()) {
        // If last solution was not the current best solution, delete it.
        if (!solWasBestSol && sol != nullptr){
            delete sol;
        }
        sol = next_sol;
        solWasBestSol = false;

        // If one asset finished, stop looking for more solutions. TODO: Make sure that search did not finish due to LNS restart limit reached etc.
        if (control.search_finished.load()) {
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
                    if (printAll){
                        sol->print(out, p);
                        out << "----------" << std::endl;
                    }
                    control.best_space_mutex.unlock();
                    
                    if (success){
                        solWasBestSol = true;
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
                            solWasBestSol = true;
                            assert(success);
                        }

                        // Memory leak from expected when swapped.
                        // if (printAll){
                            sol->print(out, p);
                            out << "----------" << std::endl;
                        // }
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
                            solWasBestSol = true;
                            assert(success);
                        }
                        // if (printAll){
                            sol->print(out, p);
                            out << "----------" << std::endl;
                        // }
                        control.best_space_mutex.unlock();

                    }
                }
                break;
            }
        }
    }
    control.assets_solve_times[asset].stop();
    control.asset_best_solutions[asset] = sol;

    // The first asset to finish will be the final best solution.
    if (!control.search_finished.exchange(true)){
        control.finished_asset = asset;
    }

    control.thread_done();
}