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
#include <deque>
#include <string>
#include <sstream>
#include <limits>
#include <unordered_set>

using namespace std;
using namespace Gecode;
using namespace Gecode::FlatZinc;

LNStrategies::LNStrategies() {
    // constructor implementation
}

LNStrategies::~LNStrategies() {
    // destructor implementation
}

bool LNStrategies::randomLNS(FlatZincSpace& fzs, MetaInfo mi, IntSharedArray& initialSolution, unsigned int lns, IntVarArgs iv_lns, Rnd random) {
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
          rel(fzs, iv_lns[i], IRT_EQ, last.iv_lns[i]);
        }
      }
      return false;
    }

    return true;
}

bool LNStrategies::pgLNS(FlatZincSpace& fzs, MetaInfo mi, IntVarArray iv, int num_non_introduced_vars, Rnd random) {
    if ((mi.type() == MetaInfo::RESTART) && (mi.restart() != 0) && (mi.last())){
      // Set up the variables to make sure that pglns stops.
      double test = 0;
      for (int i = 0; i < num_non_introduced_vars; ++i) {
        test += std::log(iv[i].size());
      }
      double stop = test * 0.90;

      // Set up the variables for the propagation guided LNS.
      std::deque<PGLNSInfo> pglns_info;
      vector<int> domainSizes(num_non_introduced_vars);
      // bool guided = false;
      int index;
      PGLNSInfo pglns_info_elem;
      IntVar curr_var;
      const FlatZincSpace& last = static_cast<const FlatZincSpace&>(*mi.last());

      while (test > stop){
        test = 0;

        if (pglns_info.size() == 0){
          index = random(num_non_introduced_vars);
          curr_var = iv[index];
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
        for (int i = 0; i < num_non_introduced_vars; ++i) {
          domainSizes[i] = iv[i].size();
        }
        // Force value accordinly, and propagate.
        rel(fzs, curr_var, IRT_EQ, last.iv[index]);
        fzs.status();
        
        // Add the variables that were propagated to pglns_info.
        for (int i = 0; i < num_non_introduced_vars; ++i) {
          int diff = domainSizes[i] - iv[i].size();
          if (diff > 0){
            pglns_info.push_back({iv[i], i, diff});
          }
        }
        // Clear the deque from last iteration (as their diffs might not be valid anymore).
        pglns_info.clear();
        // Sort the variables and indexes in iv according to the difference in domain size in pglns_info.
        std::sort(pglns_info.begin(), pglns_info.end(), [](const PGLNSInfo& a, const PGLNSInfo& b) {
          return a.domainDiff > b.domainDiff;
        });
        // Recompute new size of the domains.
        for (int i = 0; i < num_non_introduced_vars; ++i) {
            test += std::log(iv[i].size());
        }
      }

      return false;
    }
    return true;
}

bool LNStrategies::revpgLNS(FlatZincSpace& fzs, MetaInfo mi, IntVarArray iv, int num_non_introduced_vars, Rnd random) {
    if ((mi.type() == MetaInfo::RESTART) && (mi.restart() != 0) && (mi.last())){
      // Set up the variables to make sure that pglns stops.
      double test = 0;
      for (int i = 0; i < num_non_introduced_vars; ++i) {
        test += std::log(iv[i].size());
      }
      double stop = test * 0.90;

      // Set up the variables for the propagation guided LNS.
      std::deque<PGLNSInfo> pglns_info;
      vector<int> domainSizes(num_non_introduced_vars);
      // bool guided = false;
      int index;
      PGLNSInfo pglns_info_elem;
      IntVar curr_var;
      const FlatZincSpace& last = static_cast<const FlatZincSpace&>(*mi.last());

      while (test > stop){
        test = 0;

        if (pglns_info.size() == 0){
          index = random(num_non_introduced_vars);
          curr_var = iv[index];
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
        for (int i = 0; i < num_non_introduced_vars; ++i) {
          domainSizes[i] = iv[i].size();
        }
        // Force value accordinly, and propagate.
        rel(fzs, curr_var, IRT_EQ, last.iv[index]);
        fzs.status();
        
        // Add the variables that were propagated to pglns_info.
        for (int i = 0; i < num_non_introduced_vars; ++i) {
          int diff = domainSizes[i] - iv[i].size();
          if (diff > 0){
            pglns_info.push_back({iv[i], i, diff});
          }
        }
        // Clear the deque from last iteration (as their diffs might not be valid anymore).
        pglns_info.clear();
        // Sort the variables and indexes in iv according to the difference in domain size in pglns_info.
        std::sort(pglns_info.begin(), pglns_info.end(), [](const PGLNSInfo& a, const PGLNSInfo& b) {
          return a.domainDiff < b.domainDiff;
        });
        // Recompute new size of the domains.
        for (int i = 0; i < num_non_introduced_vars; ++i) {
            test += std::log(iv[i].size());
        }
      }

      return false;
    }
    return true;
}

// AFC Guided LNS Strategy
// AFC of a variable is the sum of all propagators depending on the variable + its degree
bool LNStrategies::afcLNS(FlatZincSpace& fzs, MetaInfo mi, IntVarArray iv){
  if ((mi.type() == MetaInfo::RESTART) && (mi.restart() != 0) && (mi.last())){
    const FlatZincSpace& last = static_cast<const FlatZincSpace&>(*mi.last());
    double avg_afc = 0;
    // Sum up the afc values of the variables in iv and divide by the number of variables.
    for (int i = 0; i < 12; ++i) {
      avg_afc += last.iv[i].afc();
      cerr << avg_afc << endl;
    }
    avg_afc = avg_afc / 12;

    // For each variable in iv, if the afc value is greater than the average afc value, fix the variable from the previous solution.
    for (int i = 0; i < 12; ++i) {
      if (iv[i].afc() > avg_afc){
        rel(fzs, iv[i], IRT_EQ, last.iv[i]);
      }
    }
    return false;
  }
  return true;
}

bool LNStrategies::objrelaxLNS(FlatZincSpace& fzs, MetaInfo mi, unsigned int lns, IntVarArgs iv_lns_obj_relax, Rnd random){
  if ((mi.type() == MetaInfo::RESTART) && (mi.restart() != 0) && (lns > 0) && mi.last()) {
    const FlatZincSpace& last = static_cast<const FlatZincSpace&>(*mi.last());
    for (unsigned int i=iv_lns_obj_relax.size(); i--;) {
      if (random(99U) <= lns) {
        rel(fzs, iv_lns_obj_relax[i], IRT_EQ, last.iv_lns_obj_relax[i]);
      }
    }
    return false;
  }

  return true;
}