#ifndef LNSSTRATEGIES_HH
#define LNSSTRATEGIES_HH

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

struct PGLNSInfo {
    unsigned long int ivIndex;
    int domainDiff;
};

// Structs mainly used for CIG LNS.
struct VariableShuffleInfo {
    unsigned int ivIndex;

    VariableShuffleInfo(unsigned int index) : ivIndex(index) {}
};
struct CIGInfo {
    std::vector<VariableShuffleInfo> vars;
    std::vector<double> bound_differences;
    std::vector<double> scores;
    double bound_diff_sum;
    double r;

    CIGInfo(int num_vars) : bound_differences(num_vars), scores(num_vars), bound_diff_sum(0), r(0) {
        for (int i = 0; i < num_vars; i++){
            vars.push_back(VariableShuffleInfo(i));
        }
    }
};

class LNSstrategies {
public:
    LNSstrategies(); // constructor
    ~LNSstrategies(); // destructor

    // Standard LNS
    bool random(FlatZincSpace& fzs, MetaInfo mi, std::atomic<FlatZincSpace*>* global_best_sol, IntSharedArray& initialSolution, unsigned int lns, int* iv_lns_default_idx, int idx_size, IntVarArgs iv_lns, bool use_iv_lns, Rnd random);
    // Propagation guided LNS
    bool propagationGuided(FlatZincSpace& fzs, MetaInfo mi, std::atomic<FlatZincSpace*>* global_best_sol, int* non_fzn_introduced_vars_idx, int idx_size, double lns, unsigned int queue_size, Rnd random);
    // Reversed propagation guided LNS
    bool reversedPropagationGuided(FlatZincSpace& fzs, MetaInfo mi, std::atomic<FlatZincSpace*>* global_best_sol, int* non_fzn_introduced_vars_idx, int idx_size, double lns, unsigned int queue_size, Rnd random);
    // Objective relaxation LNS
    bool objectiveRelaxation(FlatZincSpace& fzs, MetaInfo mi, std::atomic<FlatZincSpace*>* global_best_sol, unsigned int lns, int* iv_lns_obj_relax_idx, int idx_size, Rnd random);
    // Cost impact guided LNS
    bool costImpactGuided(FlatZincSpace& fzs, MetaInfo mi, std::atomic<FlatZincSpace*>* global_best_sol, CIGInfo* data, int* iv_lns_default_idx, bool maximize, unsigned int dives, double alpha, long unsigned int numfixedvars, Rnd random);
    // Static Variable Dependency LNS
    bool staticVariableRelation(FlatZincSpace& fzs, MetaInfo mi, std::atomic<FlatZincSpace*>* global_best_sol, int* non_fzn_introduced_vars_idx, int idx_size, unsigned int vars_to_fix, Rnd random);

};

#endif // LNSSTRATEGIES_HH