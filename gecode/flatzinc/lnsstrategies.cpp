// Includes
#include <iostream>
#include <fstream>
#include <gecode/flatzinc.hh>
#include <gecode/flatzinc/plugin.hh>
#include <gecode/flatzinc/branch.hh>
#include <gecode/search.hh>
#include <gecode/flatzinc/fzn-pbs.hh>
#include <gecode/flatzinc/lnsstrategies.hh>

#include <array>
#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <limits>
#include <unordered_set>
#include <cstdlib> // for rand() and srand()
#include <ctime> // for time()
#include <random>

using namespace std;
using namespace Gecode;
using namespace Gecode::FlatZinc;

LNSstrategies::LNSstrategies() {
    // constructor implementation
}

LNSstrategies::~LNSstrategies() {
    // destructor implementation
}

bool LNSstrategies::random(FlatZincSpace& fzs, MetaInfo mi, IntSharedArray& initialSolution, unsigned int lns, IntVarArgs iv_lns, Rnd random) {
    if ((mi.type() == MetaInfo::RESTART) && (mi.restart() != 0) && (lns > 0) && (mi.last()==nullptr) && (initialSolution.size()>0)) {
      for (unsigned int i=iv_lns.size(); i--;) {
        if (random(99U) <= lns) {
          rel(fzs, iv_lns[i], IRT_EQ, initialSolution[i]);
        }
      }
      return false;

    } else if ((mi.type() == MetaInfo::RESTART) && (mi.restart() != 0) && (lns > 0) && mi.last()) {
      const FlatZincSpace& last = static_cast<const FlatZincSpace&>(*mi.last());
      for (unsigned int i=iv_lns.size(); i--;) {
        if (random(99U) <= lns) {
          rel(fzs, iv_lns[i], IRT_EQ, last.iv_lns[i].val());
        }
      }
      return false;
    }

    return true;
}

bool LNSstrategies::propagationGuided(FlatZincSpace& fzs, MetaInfo mi, IntVarArray non_fzn_introduced_vars, unsigned int queue_size, Rnd random) {
    if ((mi.type() == MetaInfo::RESTART) && (mi.restart() != 0) && (mi.last())){
      // Set up the variables to make sure that pglns stops.
      double test = 0;
      for (int i = 0; i < non_fzn_introduced_vars.size(); ++i) {
        test += std::log(non_fzn_introduced_vars[i].size());
      }
      double stop = test * 0.90;

      // Set up the variables for the propagation guided LNS.
      std::deque<PGLNSInfo> pglns_info;
      vector<int> domainSizes(non_fzn_introduced_vars.size());
      // bool guided = false;
      int index;
      PGLNSInfo pglns_info_elem;
      IntVar curr_var;
      const FlatZincSpace& last = static_cast<const FlatZincSpace&>(*mi.last());

      while (test > stop){
        test = 0;

        if (pglns_info.size() == 0){
          index = random(non_fzn_introduced_vars.size());
          curr_var = non_fzn_introduced_vars[index];
          if (curr_var.assigned()){
            continue;
          }
        }
        else{
          pglns_info_elem = pglns_info.front();

          curr_var = pglns_info_elem.intVar;
          index = pglns_info_elem.ivIndex;

          pglns_info.pop_front();
        }

        // Get the domain size before the propagation
        for (int i = 0; i < non_fzn_introduced_vars.size(); ++i) {
          domainSizes[i] = non_fzn_introduced_vars[i].size();
        }
        // Force value accordinly, and propagate.
        // rel(fzs, curr_var, IRT_EQ, last.non_fzn_introduced_vars[index].val());
        rel(fzs, curr_var, IRT_EQ, last.iv_lns[index].val());
        fzs.status();
        
        // Add the variables that were propagated to pglns_info.
        for (int i = 0; i < non_fzn_introduced_vars.size(); ++i) {
          int diff = domainSizes[i] - non_fzn_introduced_vars[i].size();
          if (diff > 0 && pglns_info.size() < queue_size && index != i && !non_fzn_introduced_vars[i].assigned()){
            pglns_info.push_back({non_fzn_introduced_vars[i], i, diff});
          }
        }
        
        // Sort the variables and indexes in non_fzn_introduced_vars according to the difference in domain size in pglns_info.
        std::sort(pglns_info.begin(), pglns_info.end(), [](const PGLNSInfo& a, const PGLNSInfo& b) {
          return a.domainDiff > b.domainDiff;
        });
        // Recompute new size of the domains.
        for (int i = 0; i < non_fzn_introduced_vars.size(); ++i) {
            test += std::log(non_fzn_introduced_vars[i].size());
        }
      }
      return false;
    }
    return true;
}

bool LNSstrategies::reversedPropagationGuided(FlatZincSpace& fzs, MetaInfo mi, IntVarArray non_fzn_introduced_vars, unsigned int queue_size, Rnd random) {
    if ((mi.type() == MetaInfo::RESTART) && (mi.restart() != 0) && (mi.last())){
      // Set up the variables to make sure that pglns stops.
      double test = 0;
      for (int i = 0; i < non_fzn_introduced_vars.size(); ++i) {
        test += std::log(non_fzn_introduced_vars[i].size());
      }
      double stop = test * 0.90;

      // Set up the variables for the propagation guided LNS.
      std::deque<PGLNSInfo> pglns_info;
      vector<int> domainSizes(non_fzn_introduced_vars.size());
      // bool guided = false;
      int index;
      PGLNSInfo pglns_info_elem;
      IntVar curr_var;
      const FlatZincSpace& last = static_cast<const FlatZincSpace&>(*mi.last());

      while (test > stop){
        test = 0;

        if (pglns_info.size() == 0){
          index = random(non_fzn_introduced_vars.size());
          curr_var = non_fzn_introduced_vars[index];
          if (curr_var.assigned()){
            continue;
          }
        }
        else{
          pglns_info_elem = pglns_info.front();

          curr_var = pglns_info_elem.intVar;
          index = pglns_info_elem.ivIndex;

          pglns_info.pop_front();
        }

        // Get the domain size before the propagation
        for (int i = 0; i < non_fzn_introduced_vars.size(); ++i) {
          domainSizes[i] = non_fzn_introduced_vars[i].size();
        }
        // Force value accordinly, and propagate.
        // rel(fzs, curr_var, IRT_EQ, last.non_fzn_introduced_vars[index].val());
        rel(fzs, curr_var, IRT_EQ, last.iv_lns[index].val());
        fzs.status();
        
        // Add the variables that were propagated to pglns_info.
        for (int i = 0; i < non_fzn_introduced_vars.size(); ++i) {
          int diff = domainSizes[i] - non_fzn_introduced_vars[i].size();
          if (diff > 0 && pglns_info.size() < queue_size && index != i && !non_fzn_introduced_vars[i].assigned()){
            pglns_info.push_back({non_fzn_introduced_vars[i], i, diff});
          }
        }
        
        // Sort the variables and indexes in non_fzn_introduced_vars according to the difference in domain size in pglns_info.
        std::sort(pglns_info.begin(), pglns_info.end(), [](const PGLNSInfo& a, const PGLNSInfo& b) {
          return a.domainDiff < b.domainDiff;
        });
        // Recompute new size of the domains.
        for (int i = 0; i < non_fzn_introduced_vars.size(); ++i) {
            test += std::log(non_fzn_introduced_vars[i].size());
        }
      }
      return false;
    }
    return true;
}

bool LNSstrategies::objectiveRelaxation(FlatZincSpace& fzs, MetaInfo mi, unsigned int lns, IntVarArgs iv_lns_obj_relax, Rnd random){
  if ((mi.type() == MetaInfo::RESTART) && (mi.restart() != 0) && (lns > 0) && mi.last()) {
    const FlatZincSpace& last = static_cast<const FlatZincSpace&>(*mi.last());
    for (unsigned int i=iv_lns_obj_relax.size(); i--;) {
      if (random(99U) <= lns) {
        rel(fzs, iv_lns_obj_relax[i], IRT_EQ, last.iv_lns_obj_relax[i].val());
      }
    }
    return false;
  }

  return true;
}

bool foundBetterSolution(const FlatZincSpace& last, FlatZincSpace& current, bool maximize){
  if (maximize){
    return last.iv[last.optVar()].min() < current.iv[current.optVar()].min();
  }
  else{
    return last.iv[last.optVar()].max() > current.iv[current.optVar()].max();
  }

}

int getBound(IntVar var, bool maximize){
  if (maximize){
    return var.min();
  }
  else{
    return var.max();
  }
}

bool LNSstrategies::costImpactGuided(FlatZincSpace& fzs, MetaInfo mi, CIGInfo* data, bool maximize, unsigned int dives, double alpha, long unsigned int numfixedvars, Rnd random){
  if ((mi.type() == MetaInfo::RESTART) && (mi.restart() != 0) && mi.last()){
    const FlatZincSpace& last = static_cast<const FlatZincSpace&>(*mi.last());
    std::default_random_engine engine(random(999U));
    // Update scores and r every 10th restart or every time a better solution is found.
    if (mi.restart() == 1 || mi.restart() % 10 == 0 || foundBetterSolution(last, fzs, maximize)){
      data->bound_differences.clear();
      data->bound_differences.resize(data->vars.size(), 0);
      data->scores.clear();
      data->scores.reserve(data->vars.size());
      data->bound_diff_sum = 0;
      data->r = 0;

      FlatZincSpace* fzs_clone;
      int oldVal;
      for (unsigned int dive = 0; dive < dives; dive++){
        // Clone the space to make a dive possible.
        fzs_clone = static_cast<FlatZinc::FlatZincSpace*>(fzs.clone());
        // Create uniformly randomized permutations of the variables.
        
        std::shuffle(fzs_clone->ciglns_info->vars.begin(), fzs_clone->ciglns_info->vars.end(), engine);
        // Depending on opt method, calculate the bound differences after fixing the variables.
        for (long unsigned int i = 0; i < data->vars.size(); ++i){
          oldVal = getBound(fzs_clone->iv[fzs_clone->optVar()], maximize);
          // The variables stored in vars.intVar are those variables found in iv_lns_default.
          rel(*fzs_clone, fzs_clone->iv_lns_default[fzs_clone->ciglns_info->vars[i].ivIndex], IRT_EQ, last.iv_lns_default[fzs_clone->ciglns_info->vars[i].ivIndex].val());
          fzs_clone->status();
          double bound_diff = (abs(getBound(fzs_clone->iv[fzs_clone->optVar()], maximize) - oldVal));
          data->bound_differences[fzs_clone->ciglns_info->vars[i].ivIndex] += bound_diff;
          data->bound_diff_sum += bound_diff;
        }
        delete fzs_clone;
      }
      // Divide each element in bound differences by dives.
      for (long unsigned int i = 0; i < data->vars.size(); ++i){
        data->bound_differences[i] /= dives;
      }
      data->bound_diff_sum /= dives;

      // Compute the score for each variable.
      double score;
      for (long unsigned int i = 0; i < data->vars.size(); ++i){
        score = (alpha * data->bound_differences[i]) + ((1-alpha) * (data->vars.size()) * data->bound_diff_sum);
        data->r += score;
        data->scores[i] = score;
      }
    }

    // Select which variables to relax.
    double r_local = data->r;
    std::vector<VariableShuffleInfo> varsToRelax;
    // Use a vector of indices and shuffle it, select variables from it.
    std::vector<int> indices(data->vars.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), engine);

    double v;
    while (varsToRelax.size() < numfixedvars){
      if (r_local != 0){
        v = random((int)floor(r_local));
      }
      else{
        v = 0;
      }
      
      for (long unsigned int i = 0; i < indices.size(); ++i){
        v = v - data->scores[indices[i]];
        if (v <= 0){
          r_local = r_local - data->scores[indices[i]];
          varsToRelax.push_back(data->vars[indices[i]]);
          // More efficient than erasing the element, because vectors.
          std::swap(indices[i], indices.back());
          indices.pop_back();
          
          break;
        }
      }
    }

    // Relax the chosen variables.
    for (long unsigned int i = 0; i < varsToRelax.size(); ++i){
      unsigned int ivIndex = varsToRelax[i].ivIndex;
      rel(fzs, fzs.iv_lns_default[ivIndex], IRT_EQ, last.iv_lns_default[ivIndex].val());
    }
    // Only return false if variables were relaxed.
    return !(varsToRelax.size() > 0);

  }
  return true;
}

int selectRandomBestVar(std::vector<int> indices, int n, double** var_rels, int var_rels_size, Rnd random){
  std::vector<int> random_indices(n);
  for (int i = 0; i < n; i++){
    random_indices[i] = random(var_rels_size);
  }
  
  int current = -1;
  int best_var_index = 0;
  for (int index : random_indices){
    for (int i = 0; i < var_rels_size; i++){
      if (var_rels[index][indices[i]] > current){
        current = var_rels[index][indices[i]];
        best_var_index = index;
      }
    }
  }

  return best_var_index;
}

int selectRandomRelatedVar(std::vector<int> indices, int n, double** var_rels, int var_index, Rnd random){
  std::vector<int> random_indices(n);
  for (int i = 0; i < n; i++){
    random_indices[i] = random((int) indices.size());
  }

  // Given indices to relations, select variable with best relations.
  int best_var_index = 0;
  int current = -1;
  for (long unsigned int i = 0; i < random_indices.size(); i++){
    if (var_rels[var_index][indices[random_indices[i]]] > current){
      current = var_rels[var_index][indices[random_indices[i]]];
      best_var_index = random_indices[i];
    }
  }

  return best_var_index;
}

bool LNSstrategies::staticVariableRelation(FlatZincSpace& fzs, MetaInfo mi, IntVarArray non_fzn_introduced_vars, unsigned int vars_to_fix, Rnd random){
  if ((mi.type() == MetaInfo::RESTART) && (mi.restart() != 0) && mi.last()){
    const FlatZincSpace& last = static_cast<const FlatZincSpace&>(*mi.last());
    std::default_random_engine engine(random(999U));

    std::vector<int> indices(non_fzn_introduced_vars.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Select random initial variable to fix given random variables and their relations.
    int var_index = selectRandomBestVar(indices, ((int)ceil(non_fzn_introduced_vars.size()*0.3)), fzs.variable_relations, non_fzn_introduced_vars.size(), random);
    unsigned int fixed_vars = 0;
    while(fixed_vars < vars_to_fix && indices.size() > 0){
      rel(fzs, non_fzn_introduced_vars[indices[var_index]], IRT_EQ, last.non_fzn_introduced_vars[indices[var_index]].val());

      // Remove frozen variable so it is not picked again.
      std::swap(indices[var_index], indices.back());
      indices.pop_back();

      // Select a new variable to freeze.
      var_index = selectRandomRelatedVar(indices, ((int)ceil(non_fzn_introduced_vars.size()*0.5)), fzs.variable_relations, var_index, random);

      fixed_vars++;
    }
    return false;
  }
  return true;
}