#include <iostream>
#include <fstream>
#include <gecode/flatzinc.hh>
#include <gecode/flatzinc/registry.hh>
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
BranchModifier::BranchModifier(bool do_opposite_branching, bool use_pbs_branching) 
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
      def_float_valsel(do_opposite_branching ? FLOAT_VAL_SPLIT_MAX() : FLOAT_VAL_SPLIT_MIN()),
#endif
      use_pbs_branching(use_pbs_branching),
      pbs_variable_branchings(nullptr) {}

// Destructor
BranchModifier::~BranchModifier() {}

void addBranchAnnotation(AST::Array* pbs_variable_branchings, AST::Array* vars, std::string variable_selection, std::string value_selection) {
    AST::Array* args = new AST::Array(4);
    args->a[0] = new AST::Array(vars->a.size());
    for (long unsigned int i = 0; i < vars->a.size(); i++) {
        args->a[0]->getArray()->a[i] = new AST::IntVar(vars->a[i]->getIntVar());
    }

    args->a[1] = new AST::Atom(variable_selection);
    args->a[2] = new AST::Atom(value_selection);

    AST::Node* ann = new AST::Call("int_search", args);

    pbs_variable_branchings->getArray()->a.push_back(ann);

}

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

void BranchModifier::PBAssetBranching(std::vector<ConExpr*> constraints){
    // Allocate the annotation.
    pbs_variable_branchings = new AST::Array();
    
    for (ConExpr* ce : constraints){
        if (ce->id == "fzn_all_different_int" || ce->id == "fzn_alldifferent_except_0"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_bin_packing_load"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_bin_packing_capa" || ce->id == "fzn_bin_packing"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_circuit"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_cumulatives"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_cumulative_opt" || ce->id == "fzn_cumulative"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_diffn"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "largest", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "largest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "largest", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "largest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_global_cardinality_closed" || ce->id == "fzn_global_cardinality"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_global_cardinality_low_up" || ce->id == "fzn_global_cardinality_low_up_closed"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_disjunctive_strict_opt" || ce->id == "fzn_disjunctive_strict"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_inverse"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_decreasing_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "largest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "largest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_table_int_reif" || ce->id == "fzn_table_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_increasing_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
            }
        }
        else if (ce->id == "fzn_sort"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
            }
        }
        else if (ce->id == "fzn_value_precede_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_count_eq_reif" || ce->id == "fzn_count_eq"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_regular"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_nvalue"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_at_least_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_roots"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_range"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_lex_less_int" || ce->id == "fzn_lex_lesseq_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_int_set_channel"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_member_int_reif" || ce->id == "fzn_member_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_at_most_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
            }
        }
        else if (ce->id == "fzn_all_equal_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_among"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
    }
}

void BranchModifier::PGLNSBranching(std::vector<ConExpr*> constraints){
    // Allocate the annotation.
    pbs_variable_branchings = new AST::Array();
    
    for (ConExpr* ce : constraints){
        if (ce->id == "fzn_all_different_int" || ce->id == "fzn_alldifferent_except_0"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_bin_packing_load"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_bin_packing_capa" || ce->id == "fzn_bin_packing"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_max");
            }
        }
        else if (ce->id == "fzn_circuit"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "occurrence", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "occurrence", "indomain_min");
            }
        }
        else if (ce->id == "fzn_cumulatives"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_cumulative_opt" || ce->id == "cumulative"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "most_constrained", "indomain_min");

                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "most_constrained", "indomain_min");

                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_diffn"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "dom_w_deg", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "dom_w_deg", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "dom_w_deg", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "dom_w_deg", "indomain_min");
            }
        }
        else if (ce->id == "fzn_global_cardinality_closed" || ce->id == "fzn_global_cardinality"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_global_cardinality_low_up" || ce->id == "fzn_global_cardinality_low_up_closed"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_disjunctive_strict_opt" || ce->id == "fzn_disjunctive_strict"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_inverse"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_decreasing_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "largest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "largest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_table_int_reif" || ce->id == "fzn_table_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_increasing_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_sort"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_value_precede_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_count_eq_reif" || ce->id == "fzn_count_eq"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_regular"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_nvalue"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_at_least_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_roots"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_range"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_lex_less_int" || ce->id == "fzn_lex_lesseq_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_int_set_channel"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_member_int_reif" || ce->id == "fzn_member_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_at_most_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "dom_w_deg", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "dom_w_deg", "indomain_min");
            }
        }
        else if (ce->id == "fzn_all_equal_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_among"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "dom_w_deg", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "dom_w_deg", "indomain_min");
            }
        }

        
    }
}

void BranchModifier::CIGLNSBranching(std::vector<ConExpr*> constraints){
    // Allocate the annotation.
    pbs_variable_branchings = new AST::Array();
    
    for (ConExpr* ce : constraints){
        if (ce->id == "fzn_all_different_int" || ce->id == "fzn_alldifferent_except_0"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_bin_packing_load"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_max");
            }
        }
        else if (ce->id == "fzn_bin_packing_capa" || ce->id == "fzn_bin_packing"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_max");
            }
        }
        else if (ce->id == "fzn_circuit"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_cumulatives"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_cumulative_opt" || ce->id == "cumulative"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_diffn"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "action_max", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "action_max", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "action_max", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "action_max", "indomain_max");
            }
        }
        else if (ce->id == "fzn_global_cardinality_closed" || ce->id == "fzn_global_cardinality"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_global_cardinality_low_up" || ce->id == "fzn_global_cardinality_low_up_closed"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_disjunctive_strict_opt" || ce->id == "fzn_disjunctive_strict"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_inverse"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_decreasing_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_max");
            }
        }
        else if (ce->id == "fzn_table_int_reif" || ce->id == "fzn_table_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_increasing_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_sort"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_value_precede_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_count_eq_reif" || ce->id == "fzn_count_eq"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_regular"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_nvalue"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_at_least_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_roots"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_range"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_lex_less_int" || ce->id == "fzn_lex_lesseq_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_int_set_channel"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_member_int_reif" || ce->id == "fzn_member_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_at_most_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_all_equal_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "action_max", "indomain_min");
            }
        }
        else if (ce->id == "fzn_among"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "action_max", "indomain_min");
            }
        }

        
    }
}

void BranchModifier::OBJRELLNSBranching(std::vector<ConExpr*> constraints){
     // Allocate the annotation.
    pbs_variable_branchings = new AST::Array();
    
    for (ConExpr* ce : constraints){
        if (ce->id == "fzn_all_different_int" || ce->id == "fzn_alldifferent_except_0"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_bin_packing_load"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_bin_packing_capa" || ce->id == "fzn_bin_packing"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_circuit"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_cumulatives"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_cumulative_opt" || ce->id == "fzn_cumulative"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_diffn"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "largest", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "largest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "largest", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "largest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_global_cardinality_closed" || ce->id == "fzn_global_cardinality"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_global_cardinality_low_up" || ce->id == "fzn_global_cardinality_low_up_closed"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_disjunctive_strict_opt" || ce->id == "fzn_disjunctive_strict"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_inverse"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_decreasing_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "largest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "largest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_table_int_reif" || ce->id == "fzn_table_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_increasing_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
            }
        }
        else if (ce->id == "fzn_sort"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
            }
        }
        else if (ce->id == "fzn_value_precede_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_count_eq_reif" || ce->id == "fzn_count_eq"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_regular"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_nvalue"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_at_least_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_roots"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_range"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_lex_less_int" || ce->id == "fzn_lex_lesseq_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_int_set_channel"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_member_int_reif" || ce->id == "fzn_member_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_at_most_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
            }
        }
        else if (ce->id == "fzn_all_equal_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_among"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
    }
}

void BranchModifier::SVRLNSBranching(std::vector<ConExpr*> constraints){
     // Allocate the annotation.
    pbs_variable_branchings = new AST::Array();
    
    for (ConExpr* ce : constraints){
        if (ce->id == "fzn_all_different_int" || ce->id == "fzn_alldifferent_except_0"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_bin_packing_load"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_bin_packing_capa" || ce->id == "fzn_bin_packing"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_circuit"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "most_constrained", "indomain_min");
            }
        }
        else if (ce->id == "fzn_cumulatives"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_cumulative_opt" || ce->id == "fzn_cumulative"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_diffn"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "largest", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "largest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "largest", "indomain_max");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[3]->getArray(), "largest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_global_cardinality_closed" || ce->id == "fzn_global_cardinality"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_global_cardinality_low_up" || ce->id == "fzn_global_cardinality_low_up_closed"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_disjunctive_strict_opt" || ce->id == "fzn_disjunctive_strict"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_inverse"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_decreasing_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "largest", "indomain_max");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "largest", "indomain_max");
            }
        }
        else if (ce->id == "fzn_table_int_reif" || ce->id == "fzn_table_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_increasing_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
            }
        }
        else if (ce->id == "fzn_sort"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "smallest", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
            }
        }
        else if (ce->id == "fzn_value_precede_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[2]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_count_eq_reif" || ce->id == "fzn_count_eq"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_regular"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_nvalue"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_at_least_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_roots"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_range"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_lex_less_int" || ce->id == "fzn_lex_lesseq_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
                
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_int_set_channel"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_member_int_reif" || ce->id == "fzn_member_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "input_order", "indomain_min");
            }
        }
        else if (ce->id == "fzn_at_most_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[1]->getArray(), "smallest", "indomain_min");
            }
        }
        else if (ce->id == "fzn_all_equal_int"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
        else if (ce->id == "fzn_among"){
            if (ce->ann != nullptr && ce->ann->getArray()->a[0]->isCall("domain")){
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
            else{
                addBranchAnnotation(pbs_variable_branchings, ce->args->a[0]->getArray(), "first_fail", "indomain_min");
            }
        }
    }
}