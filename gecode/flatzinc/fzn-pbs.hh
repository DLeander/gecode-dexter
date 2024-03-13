// fzn-pbs.hh

#ifndef FZN_PBS_HH
#define FZN_PBS_HH

// Includes
#include <gecode/flatzinc.hh>
#include <gecode/flatzinc/branch.hh>
#include <gecode/search.hh>
#include <gecode/flatzinc/searchenginebase.hh>

#include <memory>
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <limits>
#include <unordered_set>

using namespace std;
using namespace Gecode;
using namespace Gecode::FlatZinc;

class FznPbs {
public:
    enum Asset {
      USER, //< First asset is the user asset.
      LNS_USER, //< Second asset is the user asset with LNS.
      USER_OPPOSITE  //< Third asset is the user asset with opposite branching.
    };
    // Methods
    FznPbs(FlatZinc::FlatZincSpace* fg, const int assets, Printer& p); // constructor
    ~FznPbs(); // destructor
    void controller(std::ostream& out, FlatZincOptions& fopt, Support::Timer& t_total);
    // Signals that a search for a thread is finished.
    void thread_done();

    // Variables
    // Intial search space.
    FlatZinc::FlatZincSpace* fg; 

    // The number of assets.
    const int assets;

    // Search space for each asset.
    std::vector<FlatZinc::FlatZincSpace*> asset_spaces;

    // Search engine type for each asset.
    std::vector<BaseEngine*> asset_engines;

    // The printer for each asset.
    FlatZinc::Printer& p;

    /// Flag indicating that the final best solution has been found.
    std::atomic<bool> search_finished;

    // Statistics for all assets.
    std::vector<Support::Timer> asset_solve_times;
    std::vector<StatusStatistics> asset_status_stats;
    std::vector<int> asset_num_propagators;
    std::vector<Search::Options> asset_search_options;

    // The best solution found given objective value.
    std::atomic<FlatZincSpace*> best_sol;

    // Vector of all best solutions found during search.
    std::vector<FlatZincSpace*> all_best_solutions;

    // The current method.
    FlatZincSpace::Meth method;

    // A mutex lock for updating best space.
    std::mutex best_space_mutex;

    // The asset that finished the search and found the solution.
    int finished_asset;

private:
    // Waits for all threads to be done.
    void await_runners_completed();
    // Sets up the asset used by the portfolio.
    void setupPortfolioAssets(int asset, FlatZinc::Printer& p, FlatZincOptions& fopt);
    // Gives the statistics of the solution. (TODO: Make it possible to output from all engines and/or spaces)
    void solutionStatistics(FlatZincSpace* fg, BaseEngine* se, Support::Timer t_solve, StatusStatistics sstat, int n_p, std::ostream& out, Support::Timer& t_total, bool allAssetStat);
    // Set up the options for a search engine.
    Search::Options setupAssetSearchOptions(FlatZincSpace* fg, FlatZincOptions& fopt, FlatZinc::Printer& p, unsigned int c_d, unsigned int a_d, double threads, int asset, BranchModifier& bm, bool use_rbs, unsigned int restart_type, double restart_base, unsigned int restart_scale);

    // Variables
    /// Flag indicating some thread is waiting on the execution to be done.
    std::atomic<bool> execution_done_wait_started;
    /// Event for signalling that execution is done.
    Gecode::Support::Event execution_done_event;
    /// The number of test runners that are to be set up.
    std::atomic<int> running_threads;
};

#include <gecode/flatzinc.hh>
class AssetExecutor : public Gecode::Support::Runnable {
    /// The common controller for running tests
    FznPbs& control;
    /// The initial space to start in.
    FlatZinc::FlatZincSpace* fg;
    // The search engine for the asset.
    BaseEngine* se;
    // The output stream.
    std::ostream& out;
    // The options for the FlatZinc space and search.
    FlatZincOptions& fopt;

    // Printer (Stores the output variables)
    FlatZinc::Printer& p;

    // The asset for this search.
    int asset;

public:
    // Constructor
    AssetExecutor(FznPbs& control, int asset, std::ostream& out, FlatZincOptions& fopt, FlatZinc::Printer& p);
    // Run the search.
    void run(void);
};

// }} 
#endif // FZN_PBS_HH