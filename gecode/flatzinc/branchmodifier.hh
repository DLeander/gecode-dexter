#ifndef BRANCHMODIFIER_HH
#define BRANCHMODIFIER_HH

#include <iostream>
#include <fstream>
#include <gecode/flatzinc.hh>
// #include <gecode/flatzinc/registry.hh>
// #include <gecode/flatzinc/plugin.hh>
#include <gecode/flatzinc/branch.hh>
#include <gecode/search.hh>

#include <array>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <unordered_set>

using namespace std;
using namespace Gecode;
using namespace Gecode::FlatZinc;

class BranchModifier {
public:
    BranchModifier(bool do_opposite_branching);  // Constructor
    ~BranchModifier(); // Destructor

    bool do_opposite_branching;  // Flag to indicate if opposite branching is enabled.

    TieBreak<IntVarBranch> def_int_varsel;
    IntValBranch def_int_valsel;

    // IntBoolVarBranch def_intbool_varsel;
    
    TieBreak<BoolVarBranch> def_bool_varsel;
    BoolValBranch def_bool_valsel;

#ifdef GECODE_HAS_SET_VARS
    SetVarBranch def_set_varsel;
    SetValBranch def_set_valsel;
#endif

#ifdef GECODE_HAS_FLOAT_VARS
    TieBreak<FloatVarBranch> def_float_varsel;
    FloatValBranch def_float_valsel;
#endif

    // Return the opposite the current branching strategy for integer (if possible)
    IntValBranch doOppositeBranchingIntVal(string id, std::string& r0, std::string& r1, Rnd rnd);
    // Return the opposite the current branching strategy for integer (if possible)
    TieBreak<IntVarBranch> doOppositeBranchingIntVar(string id, Rnd rnd, double decay);

    // Return the opposite the current branching strategy for Booleans (if possible)
    BoolValBranch doOppositeBranchingBoolVal(string id, std::string& r0, std::string& r1, Rnd rnd);
    // Return the opposite the current branching strategy for Booleans (if possible)
    TieBreak<BoolVarBranch> doOppositeBranchingBoolVar(string id, Rnd rnd, double decay);
    
    
#ifdef GECODE_HAS_SET_VARS
    // Return the opposite the current branching strategy for sets (if possible)
    SetValBranch doOppositeBranchingSetVal(string id, std::string& r0, std::string& r1);
    // Return the opposite the current branching strategy for sets (if possible)
    SetVarBranch doOppositeBranchingSetVar(string id, Rnd rnd, double decay);
#endif
    
#ifdef GECODE_HAS_FLOAT_VARS
    // Return the opposite the current branching strategy for floats (if possible)
    FloatValBranch doOppositeBranchingFloatVal(string id, std::string& r0, std::string& r1);
    // Return the opposite the current branching strategy for floats (if possible)
    TieBreak<FloatVarBranch> doOppositeBranchingFloatVar(string id, Rnd rnd, double decay);
#endif
    
private:
};

#endif // BRANCHMODIFIER_HH