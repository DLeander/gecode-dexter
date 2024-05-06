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
    IntVar intVar;
    int ivIndex;
    int domainDiff;
};

// Structs mainly used for CIG LNS.
struct VariableShuffleInfo {
    IntVar intVar;
    unsigned int ivIndex;

    VariableShuffleInfo(IntVar var, unsigned int index) : intVar(var), ivIndex(index) {}
};
struct CIGInfo {
    std::vector<VariableShuffleInfo> vars;
    std::vector<double> bound_differences;
    std::vector<double> scores;
    double bound_diff_sum;
    double r;

    CIGInfo(IntVarArray lns_vars) : bound_differences(lns_vars.size()), scores(lns_vars.size()), bound_diff_sum(0), r(0) {
        for (int i = 0; i < lns_vars.size(); i++){
            vars.push_back(VariableShuffleInfo(lns_vars[i], i));
        }
    }
};

class LNSstrategies {
public:
    LNSstrategies(); // constructor
    ~LNSstrategies(); // destructor

    // Standard LNS
    bool random(FlatZincSpace& fzs, MetaInfo mi, IntSharedArray& initialSolution, unsigned int lns, IntVarArgs iv_lns, Rnd random);
    // Propagation guided LNS
    bool propagationGuided(FlatZincSpace& fzs, MetaInfo mi, IntVarArray non_fzn_introduced_vars, unsigned int queue_size, Rnd random);
    // Reversed propagation guided LNS
    bool reversedPropagationGuided(FlatZincSpace& fzs, MetaInfo mi, IntVarArray non_fzn_introduced_vars, unsigned int queue_size, Rnd random);
    // Objective relaxation LNS
    bool objectiveRelaxation(FlatZincSpace& fzs, MetaInfo mi, unsigned int lns, IntVarArgs iv_lns_obj_relax, Rnd random);
    // Cost impact guided LNS
    bool costImpactGuided(FlatZincSpace& fzs, MetaInfo mi, CIGInfo* data, bool maximize, unsigned int dives, double alpha, long unsigned int numfixedvars, Rnd random);
    // Static Variable Dependency LNS
    bool staticVariableRelation(FlatZincSpace& fzs, MetaInfo mi, IntVarArray non_fzn_introduced_vars, unsigned int vars_to_fix, Rnd random);

};

#endif // LNSSTRATEGIES_HH