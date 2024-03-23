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
#include <gecode/flatzinc/searchenginebase.hh>
#include <gecode/flatzinc/branchmodifier.hh>

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
      p(p), 
      search_finished(false), 
      asset_solve_times(assets), 
      asset_status_stats(assets), 
      asset_num_propagators(assets),
      asset_search_options(assets),
      best_sol(nullptr), 
      finished_asset(-1)
    {
    execution_done_wait_started = false;
    running_threads = assets;
    // Initialize global_objective given method:
    method = fg->method();
}

FznPbs::~FznPbs() {
    for (long unsigned int i = 0; i < all_best_solutions.size(); i++){
        if (all_best_solutions[i] != nullptr){
            delete all_best_solutions[i];
            all_best_solutions[i] = nullptr;
        }
    }
    for (int i = 0; i < assets; i++) {
        delete asset_search_options[i].stop;
        delete asset_search_options[i].tracer;
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
            n_p = asset_num_propagators[asset];
            out << "%%%mzn-stat: unfinished asset="
                << asset << std::endl;
            out << "%%%mzn-stat: propagators=" << n_p << std::endl
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
Search::Options FznPbs::setupAssetSearchOptions(FlatZincSpace* fg, FlatZincOptions& fopt, FlatZinc::Printer& p, unsigned int c_d, unsigned int a_d, double threads, int asset, BranchModifier& bm, bool use_rbs = false, FlatZinc::FlatZincSpace::LNSType lns_type = FlatZinc::FlatZincSpace::LNSType::RANDOM, unsigned int restart_type = 1, double restart_base = 1.5, unsigned int restart_scale = 250){
    
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

    if (fopt.interrupt()) Driver::PBSCombinedStop::installCtrlHandler(true);

    // Set the type of LNS in the FlatZinc space given parameter.
    if (use_rbs){
        fg->setLNSType(lns_type);
    }

    // If not RBS but asset is to use it:
    if (fopt.restart() == RM_NONE && use_rbs){
        fopt.restart(RestartMode(restart_type));
        fopt.restart_base(restart_base);
        fopt.restart_scale(restart_scale);

        search_options.cutoff = new Search::CutoffAppend(new Search::CutoffConstant(0), 1, Driver::createCutoff(fopt));
        // Create the branchers. (Needs to be here due to non-rbs options and assets that may utilize rbs.)
        fg->createBranchers(p, fg->solveAnnotations(), fopt, false, bm, std::cerr);
        // Set fopt back to RM_NONE.
        fopt.restart(RM_NONE);
    }
    // If default option is RBS but asset is not to use it:
    else if (fopt.restart() != RM_NONE && !use_rbs){
        int restart_type = fopt.restart();
        fopt.restart(RM_NONE);
        search_options.cutoff = new Search::CutoffAppend(new Search::CutoffConstant(0), 1, Driver::createCutoff(fopt));

        fg->createBranchers(p, fg->solveAnnotations(), fopt, false, bm, std::cerr);
        fopt.restart(RestartMode(restart_type));
    }
    // Else default:
    else{
        search_options.cutoff = new Search::CutoffAppend(new Search::CutoffConstant(0), 1, Driver::createCutoff(fopt));
        fg->createBranchers(p, fg->solveAnnotations(), fopt, false, bm, std::cerr);
    }

    if (asset == assets){
        fg->shrinkArrays(p);
    }

    return search_options;
}

void FznPbs::setupPortfolioAssets(int asset, FlatZinc::Printer& p, FlatZincOptions& fopt) {
    // Set up the portfolio assets.
    asset_spaces[asset] = static_cast<FlatZinc::FlatZincSpace*>(fg->clone());
    // Set the shared current best solutions between assets for each asset.
    asset_spaces[asset]->pbs_current_best_sol = &best_sol;
    // Set the solve annotations for the asset, as it does not follow from the clone.
    asset_spaces[asset]->setSolveAnnotations(fg->solveAnnotations());

    // Copy iv,bv,sv_introduced vector from fg, as it does not follow the cloning process.
    asset_spaces[asset]->iv_introduced = fg->iv_introduced;
    asset_spaces[asset]->bv_introduced = fg->bv_introduced;
    asset_spaces[asset]->sv_introduced = fg->sv_introduced;

    Search::Options search_options;
    switch (Asset(asset))
    // Depending on what asset, set up the engine for the asset and the specific brancher.
    {
    // Base asset, the same as the one given by the user.
    case USER:
    {
        BranchModifier bm(false);
        search_options = setupAssetSearchOptions(asset_spaces[asset], fopt, p, fopt.c_d(), fopt.a_d(), fopt.threads(), asset, bm);
        asset_engines[asset] = new BABEngine(asset_spaces[asset], search_options);
        break;
    }
    // Second asset, LNS with base options. (LNS for now)
    case LNS_USER:
    {
        BranchModifier bm(false);
        asset_spaces[asset]->setSolveAnnotations(fg->solveAnnotations());
        search_options = setupAssetSearchOptions(asset_spaces[asset], fopt, p, fopt.c_d(), fopt.a_d(), fopt.threads(), asset, bm, true);
        asset_engines[asset] = new RBSEngine(asset_spaces[asset], search_options);
        break;
    }
    // Third asset opposite branching (for now)
    case USER_OPPOSITE:
    {
        BranchModifier bm(true);
        search_options = setupAssetSearchOptions(asset_spaces[asset], fopt, p, fopt.c_d(), fopt.a_d(), fopt.threads(), asset, bm);
        asset_engines[asset] = new BABEngine(asset_spaces[asset], search_options);
        break;
    }
    // Propagation Guided LNS.
    case PGLNS:
    {
        BranchModifier bm(false);
        search_options = setupAssetSearchOptions(asset_spaces[asset], fopt, p, fopt.c_d(), fopt.a_d(), fopt.threads(), asset, bm, true, FlatZinc::FlatZincSpace::LNSType::PG);
        asset_engines[asset] = new RBSEngine(asset_spaces[asset], search_options);
        break;
        
    }
    default:
        break;
    }

    asset_search_options[asset] = search_options;
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
        Gecode::Support::Thread::run(new AssetExecutor(*this, asset, out, fopt, p));
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

    // If print Statistics:
    if (fopt.mode() == SM_STAT) {
        solutionStatistics(sol, se, asset_solve_times[finished_asset], asset_status_stats[finished_asset], asset_num_propagators[finished_asset], out, t_total, fopt.fullStatistics());
    }
}


// ########################################################################
//                         AssetExecutor below.
// ########################################################################

AssetExecutor::AssetExecutor(FznPbs& control, int asset, std::ostream& out, FlatZincOptions& fopt, FlatZinc::Printer& p)
    : control(control), fg(control.asset_spaces[asset]), se(control.asset_engines[asset]), out(out), fopt(fopt), p(p), asset(asset) {}

void AssetExecutor::run(void){
    // Start the search timer.
    control.asset_solve_times[asset].start();
    StatusStatistics sstat;
    if (fg->status(sstat) != SS_FAILED) {
        control.asset_num_propagators[asset] = PropagatorGroup::all.size(*fg);
        control.asset_status_stats[asset] = sstat;
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
        // If one asset finished, stop looking for more solutions. TODO: Make sure that search did not finish due to LNS restart limit reached etc.
        if (control.search_finished.load()) {
            delete next_sol;
            break;
        }


        sol = next_sol;
        solWasBestSol = false;
        int optVar = sol->optVar();
        while(true){
            FlatZincSpace* control_best_sol = control.best_sol.load();

            if (control_best_sol == nullptr){
                    control.best_space_mutex.lock();
                    // Critical Section
                    FlatZincSpace* expected = nullptr;
                    bool success = control.best_sol.compare_exchange_strong(expected, sol);
                    if (success){
                        control.all_best_solutions.push_back(sol);
                        if (printAll){
                            sol->print(out, p);
                            out << "----------" << std::endl;
                        }
                        solWasBestSol = true;
                    }
                    control.best_space_mutex.unlock();
                    
                    if (success){
                        break;
                    }
            }
            else{
                // TODO: Does not handle float yet.
                if (control.method == FlatZincSpace::MAX){
                    if (control_best_sol->iv[optVar].val() < sol->iv[optVar].val()){
                        control.best_space_mutex.lock();
                        // Critical Section
                        FlatZincSpace* expected = control.best_sol.load();
                        if (expected->iv[optVar].val() < sol->iv[optVar].val()){
                            bool success = control.best_sol.compare_exchange_strong(expected, sol);
                            assert(success);
                            solWasBestSol = true;
                        }
                        control.all_best_solutions.push_back(sol);
                        if (printAll){
                            sol->print(out, p);
                            out << "----------" << std::endl;
                        }
                        control.best_space_mutex.unlock();
                    }
                }
                else if (control.method == FlatZincSpace::MIN){
                    if (control_best_sol->iv[optVar].val() > sol->iv[optVar].val()){
                        control.best_space_mutex.lock();
                        // Critical Section
                        FlatZincSpace* expected = control.best_sol.load();
                        if (expected->iv[optVar].val() > sol->iv[optVar].val()){
                            bool success = control.best_sol.compare_exchange_strong(expected, sol);
                            assert(success);
                            solWasBestSol = true;
                        }
                        control.all_best_solutions.push_back(sol);
                        if (printAll){
                            sol->print(out, p);
                            out << "----------" << std::endl;
                        }
                        control.best_space_mutex.unlock();
                    }
                }

                break;
            }
        }
    }
    control.asset_solve_times[asset].stop();

    // The first asset to finish will be the final best solution.
    if (!control.search_finished.exchange(true)){
        control.finished_asset = asset;
    }
    else{
        delete sol;
        sol = nullptr;
    }

    control.thread_done();
}