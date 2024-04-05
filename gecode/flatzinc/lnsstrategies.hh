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

class LNStrategies {
public:
    LNStrategies(); // constructor
    ~LNStrategies(); // destructor

    // Standard LNS
    bool randomLNS(FlatZincSpace& fzs, MetaInfo mi, IntSharedArray& initialSolution, unsigned int lns, IntVarArgs iv_lns, Rnd random);
    bool pgLNS(FlatZincSpace& fzs, MetaInfo mi, IntVarArray iv, int num_non_introduced_vars, Rnd random);
    bool revpgLNS(FlatZincSpace& fzs, MetaInfo mi, IntVarArray iv, int num_non_introduced_vars, Rnd random);
    bool afcLNS(FlatZincSpace& fzs, MetaInfo mi, IntVarArray iv);
    bool objrelaxLNS(FlatZincSpace& fzs, MetaInfo mi, unsigned int lns, IntVarArgs iv_lns_obj_relax, Rnd random);

};

#endif // LNSSTRATEGIES_HH