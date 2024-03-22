#include <iostream>
#include <fstream>
#include <gecode/flatzinc.hh>
// #include <gecode/flatzinc/registry.hh>
// #include <gecode/flatzinc/plugin.hh>
#include <gecode/search.hh>
#include <gecode/flatzinc/branch.hh>
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

// Constructor
BranchModifier::BranchModifier(bool do_opposite_branching) 
    : do_opposite_branching(do_opposite_branching),
      def_int_varsel(do_opposite_branching ? INT_VAR_AFC_SIZE_MIN(0.99) : INT_VAR_AFC_SIZE_MAX(0.99)),
      def_int_valsel(do_opposite_branching ? INT_VAL_MAX() : INT_VAL_MIN()),
    //   def_intbool_varsel(do_opposite_branching ? INTBOOL_VAR_AFC_SIZE_MAX(0.99) : INTBOOL_VAR_AFC_SIZE_MAX(0.99)),
      def_bool_varsel(do_opposite_branching ? BOOL_VAR_AFC_MIN(0.99) : BOOL_VAR_AFC_MAX(0.99)),
      def_bool_valsel(do_opposite_branching ? BOOL_VAL_MAX() : BOOL_VAL_MIN())
#ifdef GECODE_HAS_SET_VARS
      ,def_set_varsel(do_opposite_branching ? SET_VAR_AFC_SIZE_MIN(0.99) : SET_VAR_AFC_SIZE_MAX(0.99)),
      def_set_valsel(do_opposite_branching ? SET_VAL_MAX_INC() : SET_VAL_MIN_INC())
#endif
#ifdef GECODE_HAS_FLOAT_VARS
      ,def_float_varsel(do_opposite_branching ? FLOAT_VAR_SIZE_MAX() : FLOAT_VAR_SIZE_MIN()),
      def_float_valsel(do_opposite_branching ? FLOAT_VAL_SPLIT_MAX() : FLOAT_VAL_SPLIT_MIN())
#endif
{}

// Destructor
BranchModifier::~BranchModifier() {}

TieBreak<IntVarBranch> BranchModifier::doOppositeBranchingIntVar(string id, Rnd rnd, double decay) {
    if (id == "input_order")
        return TieBreak<IntVarBranch>(INT_VAR_NONE());

      if (id == "first_fail")
        return TieBreak<IntVarBranch>(INT_VAR_SIZE_MAX());

      if (id == "anti_first_fail")
        return TieBreak<IntVarBranch>(INT_VAR_SIZE_MIN());

      if (id == "smallest")
        return TieBreak<IntVarBranch>(INT_VAR_MAX_MAX());

      if (id == "largest")
        return TieBreak<IntVarBranch>(INT_VAR_MIN_MIN());

      if (id == "occurrence")
        return TieBreak<IntVarBranch>(INT_VAR_DEGREE_MAX());

      if (id == "max_regret")
        return TieBreak<IntVarBranch>(INT_VAR_REGRET_MIN_MAX());

      if (id == "most_constrained")
        return TieBreak<IntVarBranch>(INT_VAR_SIZE_MAX(), INT_VAR_DEGREE_MAX());

      if (id == "random")
        return TieBreak<IntVarBranch>(INT_VAR_RND(rnd));

      if (id == "dom_w_deg")
        return TieBreak<IntVarBranch>(INT_VAR_AFC_SIZE_MAX(decay));

      if (id == "afc_min")
        return TieBreak<IntVarBranch>(INT_VAR_AFC_MAX(decay));

      if (id == "afc_max")
        return TieBreak<IntVarBranch>(INT_VAR_AFC_MIN(decay));
        
      if (id == "afc_size_min")
        return TieBreak<IntVarBranch>(INT_VAR_AFC_SIZE_MAX(decay));
        
      if (id == "afc_size_max")
        return TieBreak<IntVarBranch>(INT_VAR_AFC_SIZE_MIN(decay));

      if (id == "action_min")
        return TieBreak<IntVarBranch>(INT_VAR_ACTION_MAX(decay));

      if (id == "action_max")
        return TieBreak<IntVarBranch>(INT_VAR_ACTION_MIN(decay));

      if (id == "action_size_min")
        return TieBreak<IntVarBranch>(INT_VAR_ACTION_SIZE_MAX(decay));
        
      if (id == "action_size_max")
        return TieBreak<IntVarBranch>(INT_VAR_ACTION_SIZE_MIN(decay));

    return TieBreak<IntVarBranch>(INT_VAR_NONE()); 
}

IntValBranch BranchModifier::doOppositeBranchingIntVal(string id, std::string& r0, std::string& r1, Rnd rnd) {
    if (id == "indomain_min") {
        r0 = "="; r1 = "!=";
        return INT_VAL_MAX();
      }
    if (id == "indomain_max") {
        r0 = "="; r1 = "!=";
        return INT_VAL_MIN();
    }
    if (id == "indomain_median") {
        r0 = "="; r1 = "!=";
        return INT_VAL_MED();
    }
    if (id == "indomain_split") {
        r0 = ">"; r1 = "<=";
        return INT_VAL_SPLIT_MAX();
    }
    if (id == "indomain_reverse_split") {
        r0 = "<="; r1 = ">";
        return INT_VAL_SPLIT_MIN();
    }
    if (id == "indomain_random") {
        r0 = "="; r1 = "!=";
        return INT_VAL_RND(rnd);
    }
    if (id == "indomain") {
        r0 = "="; r1 = "=";
        return INT_VALUES_MIN();
    }
    if (id == "indomain_middle") {
        std::cerr << "Warning, replacing unsupported annotation "
                  << "indomain_middle with indomain_median" << std::endl;
        r0 = "="; r1 = "!=";
        return INT_VAL_MED();
    }
    if (id == "indomain_interval") {
        std::cerr << "Warning, replacing unsupported annotation "
                  << "indomain_interval with indomain_split" << std::endl;
        r0 = "<="; r1 = ">";
        return INT_VAL_SPLIT_MAX();
    }

    return INT_VAL_MIN();
}


TieBreak<BoolVarBranch> BranchModifier::doOppositeBranchingBoolVar(string id, Rnd rnd, double decay) {
    if ((id == "input_order") || (id == "first_fail") || (id == "anti_first_fail") || (id == "smallest") || (id == "largest") || (id == "max_regret"))
        return TieBreak<BoolVarBranch>(BOOL_VAR_NONE());

    if ((id == "occurrence") || (id == "most_constrained"))
        return TieBreak<BoolVarBranch>(BOOL_VAR_DEGREE_MAX());

    if (id == "random")
        return TieBreak<BoolVarBranch>(BOOL_VAR_RND(rnd));

    if ((id == "afc_min") || (id == "afc_size_min"))
        return TieBreak<BoolVarBranch>(BOOL_VAR_AFC_MAX(decay));

    if ((id == "afc_max") || (id == "afc_size_max") || (id == "dom_w_deg"))
        return TieBreak<BoolVarBranch>(BOOL_VAR_AFC_MIN(decay));

    if ((id == "action_min") && (id == "action_size_min"))
        return TieBreak<BoolVarBranch>(BOOL_VAR_ACTION_MAX(decay)); 

    if ((id == "action_max") || (id == "action_size_max"))
        return TieBreak<BoolVarBranch>(BOOL_VAR_ACTION_MIN(decay));

    return TieBreak<BoolVarBranch>(BOOL_VAR_NONE());
}
BoolValBranch BranchModifier::doOppositeBranchingBoolVal(string id, std::string& r0, std::string& r1, Rnd rnd) {
    if (id == "indomain_min") {
        r0 = "="; r1 = "!=";
        return BOOL_VAL_MAX();
    }
    if (id == "indomain_max") {
        r0 = "="; r1 = "!=";
        return BOOL_VAL_MIN();
    }
    if (id == "indomain_median") {
        r0 = "="; r1 = "!=";
        return BOOL_VAL_MIN();
    }
    if (id == "indomain_split") {
        r0 = ">"; r1 = "<=";
        return BOOL_VAL_MAX();
    }
    if (id == "indomain_reverse_split") {
        r0 = "<="; r1 = ">";
        return BOOL_VAL_MIN();
    }
    if (id == "indomain_random") {
        r0 = "="; r1 = "!=";
        return BOOL_VAL_RND(rnd);
    }
    if (id == "indomain") {
        r0 = "="; r1 = "=";
        return BOOL_VAL_MIN();
    }
    if (id == "indomain_middle") {
        std::cerr << "Warning, replacing unsupported annotation "
                    << "indomain_middle with indomain_median" << std::endl;
        r0 = "="; r1 = "!=";
        return BOOL_VAL_MIN();
    }
    if (id == "indomain_interval") {
        std::cerr << "Warning, replacing unsupported annotation "
                    << "indomain_interval with indomain_split" << std::endl;
        r0 = "<="; r1 = ">";
        return BOOL_VAL_MIN();
    }

    return BOOL_VAL_MIN();
}

#ifdef GECODE_HAS_SET_VARS
SetVarBranch BranchModifier::doOppositeBranchingSetVar(string id, Rnd rnd, double decay) {
    if (id == "input_order")
        return SET_VAR_NONE();

    if (id == "first_fail")
        return SET_VAR_SIZE_MAX();

    if (id == "anti_first_fail")
        return SET_VAR_SIZE_MIN();

    if (id == "smallest")
        return SET_VAR_MAX_MAX();

    if (id == "largest")
        return SET_VAR_MIN_MIN();

    if (id == "afc_min")
        return SET_VAR_AFC_MAX(decay);

    if (id == "afc_max")
        return SET_VAR_AFC_MIN(decay);

    if (id == "afc_size_min")
        return SET_VAR_AFC_SIZE_MAX(decay);

    if (id == "afc_size_max")
        return SET_VAR_AFC_SIZE_MIN(decay);

    if (id == "action_min")
        return SET_VAR_ACTION_MAX(decay);

    if (id == "action_max")
       return SET_VAR_ACTION_MIN(decay);

    if (id == "action_size_min")
        return SET_VAR_ACTION_SIZE_MAX(decay);
       
    if (id == "action_size_max")
        return SET_VAR_ACTION_SIZE_MIN(decay);

    if (id == "random") {
        return SET_VAR_RND(rnd);
    }

    return SET_VAR_NONE();
}
SetValBranch BranchModifier::doOppositeBranchingSetVal(string id, std::string& r0, std::string& r1) {
    if (id == "indomain_min") {
        r0 = "in"; r1 = "not in";
        return SET_VAL_MAX_INC();
    }
    if (id == "indomain_max") {
        r0 = "in"; r1 = "not in";
        return SET_VAL_MIN_INC();
    }
    if (id == "outdomain_min") {
        r1 = "in"; r0 = "not in";
        return SET_VAL_MAX_EXC();
    }
    if (id == "outdomain_max") {
        r1 = "in"; r0 = "not in";
        return SET_VAL_MIN_EXC();
    }

    return SET_VAL_MIN_INC();
}
#endif

#ifdef GECODE_HAS_FLOAT_VARS
TieBreak<FloatVarBranch> BranchModifier::doOppositeBranchingFloatVar(string id, Rnd rnd, double decay) {
    if (id == "input_order")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_NONE());

    if (id == "first_fail")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_SIZE_MAX());

    if (id == "anti_first_fail")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_SIZE_MIN());

    if (id == "smallest")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_MAX_MAX());
        
    if (id == "largest")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_MIN_MIN());

    if (id == "occurrence")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_DEGREE_MAX());

    if (id == "most_constrained")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_SIZE_MAX(), FLOAT_VAR_DEGREE_MAX());

    if (id == "random")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_RND(rnd));

    if (id == "afc_min")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_AFC_MAX(decay));
        
    if (id == "afc_max")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_AFC_MIN(decay));

    if (id == "afc_size_min")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_AFC_SIZE_MAX(decay));

    if (id == "afc_size_max")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_AFC_SIZE_MIN(decay));

    if (id == "action_min")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_ACTION_MAX(decay));
        
    if (id == "action_max")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_ACTION_MIN(decay));

    if (id == "action_size_min")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_ACTION_SIZE_MAX(decay));

    if (id == "action_size_max")
        return TieBreak<FloatVarBranch>(FLOAT_VAR_ACTION_SIZE_MIN(decay));

    return TieBreak<FloatVarBranch>(FLOAT_VAR_NONE());
}

FloatValBranch BranchModifier::doOppositeBranchingFloatVal(string id, std::string& r0, std::string& r1) {
    if (id == "indomain_split") {
        r0 = "<="; r1 = ">";
        return FLOAT_VAL_SPLIT_MAX();
    }
    if (id == "indomain_reverse_split") {
        r1 = "<="; r0 = ">";
        return FLOAT_VAL_SPLIT_MIN();
    }

    return FLOAT_VAL_SPLIT_MIN();
}
#endif