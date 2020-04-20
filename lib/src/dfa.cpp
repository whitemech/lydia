/*
 * This file is part of Lydia.
 *
 * Lydia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Lydia is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Lydia.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "dfa.hpp"
#include <cuddObj.hh>
#include <exception>
#include <utils/misc.hpp>
#include <utils/strings.hpp>

namespace whitemech {
namespace lydia {

Logger dfa::logger = Logger("dfa");

dfa::dfa(CUDD::Cudd *mgr, int nb_variables) : mgr{mgr} {
  this->nb_variables = nb_variables;
  nb_bits = 1;
  nb_states = 1;

  //  add BDD variables for all the variables
  for (int i = 0; i < nb_bits + nb_variables; i++) {
    CUDD::BDD b = mgr->bddVar();
    bddvars.push_back(b);
  }

  // state 0 is a sink state - put a self-loop.
  CUDD::BDD zero = mgr->bddZero();
  root_bdds.push_back(zero);

  //  start with empty set of final states.
  finalstatesBDD = mgr->bddZero();
}

void dfa::bdd2dot(const std::string &directory) {
  for (int i = 0; i < root_bdds.size(); i++) {
    std::string filename = directory + "/" + std::to_string(i);
    dumpdot(root_bdds[i], filename);
    std::system(("dot -Tsvg " + filename + " > " + filename + ".svg").c_str());
  }
}

// return positive or negative bdd variable index
CUDD::BDD dfa::var2bddvar(int v, int index) {
  if (v == 0) {
    return !bddvars[index];
  } else {
    return bddvars[index];
  }
}

void dfa::construct_bdd_from_mona(
    const std::vector<std::vector<int>> &mona_bdd_nodes,
    const std::vector<int> &behaviour, const std::vector<int> &final_states) {
  // Create all the variables, the ones for the bits of the states and the ones
  // for the variables.
  auto tBDD = std::vector<vec_bdd>(mona_bdd_nodes.size());
  for (int i = 0; i < nb_bits + nb_variables; i++) {
    CUDD::BDD b = mgr->bddVar();
    bddvars.push_back(b);
  }
  // For each bit of the state, create a BDD zero constant
  // Each var will be updated later in 'OR'
  for (int i = 0; i < nb_bits; i++) {
    CUDD::BDD d = mgr->bddZero();
    root_bdds.push_back(d);
  }
  // populate the tBDD mapping.
  // Each item is associated to one MONA BDD node (both leaves and variables),
  // and contains a vector of BDDs.
  for (int i = 0; i < tBDD.size(); i++) {
    if (tBDD[i].empty()) {
      vec_bdd b = try_get(i, mona_bdd_nodes, tBDD);
    }
  }
  // Assign each state to the next BDD node.
  // Basically, this for loop processes the content of the MONA DFA 'behaviour'
  // vector.
  for (int i = 0; i < nb_bits; i++) {
    for (int j = 0; j < nb_states; j++) {
      // Build the BDD representation of a state.
      // we set the temporary variable to 1 because we are going to put it in
      // 'AND'.
      CUDD::BDD tmp = mgr->bddOne();
      // the first for-loop handles the most significant bits
      // that are not covered by the binary representation.
      // the second for-loop handles all the other bits.
      // TODO refactor by using the 'nb_fill_bits' parameter of state2bin.
      std::string bins = state2bin(j);
      int offset = nb_bits - bins.size();
      for (int m = 0; m < offset; m++) {
        tmp = tmp * var2bddvar(0, m);
      }
      for (int m = 0; m < bins.size(); m++) {
        // from char '1' (or '0') to int 1 (or 0)
        auto bit_mth = int(bins[m]) - 48;
        tmp = tmp * var2bddvar(bit_mth, offset + m);
      }
      // Now, put in 'AND' with the correct
      // BDD node (either terminal/leaf or variable)
      // At the bit 'i'.
      tmp = tmp * tBDD[behaviour[j]][i];
      // Put each root bdd in OR with the current temporary BDD.
      root_bdds[i] = root_bdds[i] + tmp;
    }
  }

  // Create the BDD for recognizing the accepting states.
  finalstatesBDD = mgr->bddZero();
  for (int finalstate : final_states) {
    CUDD::BDD ac = state2bdd(finalstate);
    finalstatesBDD += ac;
  }
}

CUDD::BDD dfa::state2bdd(int s) {
  std::string bin = state2bin(s);
  CUDD::BDD b = mgr->bddOne();
  int nzero = nb_bits - bin.length();
  // std::cout<<nzero<<std::endl;
  for (int i = 0; i < nzero; i++) {
    b *= !bddvars[i];
  }
  for (int i = 0; i < bin.length(); i++) {
    if (bin[i] == '0')
      b *= !bddvars[i + nzero];
    else
      b *= bddvars[i + nzero];
  }
  return b;
}

vec_bdd dfa::try_get(int index,
                     const std::vector<std::vector<int>> &mona_bdd_nodes,
                     std::vector<vec_bdd> &tBDD) {
  if (!tBDD[index].empty())
    return tBDD[index];
  vec_bdd b;
  if (mona_bdd_nodes[index][0] == -1) {
    // case when BDD node is a leaf
    // the format is: "-1 val 0"
    int value = mona_bdd_nodes[index][1];
    // get the binary representation of the state
    std::string bins = state2bin(value);
    // this representation may use less bits
    // than all the bits pre-allocated.
    // Set to zero the bits in excess.
    for (int m = 0; m < nb_bits - bins.size(); m++) {
      CUDD::BDD temp_bdd = mgr->bddZero();
      b.push_back(temp_bdd);
    }
    // Populate the LSBs
    for (char bin : bins) {
      if (bin == '0') {
        CUDD::BDD temp_bdd = mgr->bddZero();
        b.push_back(temp_bdd);
      } else if (bin == '1') {
        CUDD::BDD temp_bdd = mgr->bddOne();
        b.push_back(temp_bdd);
      } else {
        logger.error("error binary state");
        assert(false);
      }
    }
    tBDD[index] = b;
    return b;
  } else {
    // case when BDD node is an intermediate node
    // root index != -1
    int rootindex = mona_bdd_nodes[index][0];
    int leftindex = mona_bdd_nodes[index][1];
    int rightindex = mona_bdd_nodes[index][2];
    CUDD::BDD root = bddvars[nb_bits + rootindex];
    vec_bdd low = try_get(leftindex, mona_bdd_nodes, tBDD);
    vec_bdd high = try_get(rightindex, mona_bdd_nodes, tBDD);
    assert(low.size() == high.size());
    assert(low.size() == nb_bits);
    for (int i = 0; i < low.size(); i++) {
      // TODO old comment said "Assuming this is correct". Investigate.
      CUDD::BDD tmp = root.Ite(high[i], low[i]);
      b.push_back(tmp);
    }
    tBDD[index] = b;
    return b;
  }
}

void dfa::dumpdot(CUDD::BDD &b, const std::string &filename) {
  FILE *fp = fopen(filename.c_str(), "w");
  std::vector<CUDD::BDD> single(1);
  single[0] = b;
  this->mgr->DumpDot(single, nullptr, nullptr, fp);
  fclose(fp);
}

dfa *dfa::read_from_file(const std::string &filename, CUDD::Cudd *mgr) {
  int nb_variables = -1;
  std::vector<std::string> variables;
  int nb_states = -1;
  int initial_state = -1;
  int nb_nodes = -1;
  std::vector<item> mona_bdd_nodes;
  std::vector<int> final_states;
  std::vector<int> behaviour;

  std::ifstream f(filename.c_str());
  if (f.is_open()) {
    bool flag = 0;
    std::string line;
    item tmp;                        // item: vector<int>
    std::vector<std::string> fields; // temporary varibale

    while (getline(f, line)) {
      if (flag == 0) {
        if (strfind(line, "number of variables")) {
          fields = split(line, " +");
          nb_variables = stoi(fields[3]);
          whitemech::lydia::dfa::logger.debug("number of variables: {}",
                                              nb_variables);
        }
        if (strfind(line, "variables") && !strfind(line, "number")) {
          variables = split(line, " +");
          variables.erase(variables.begin());
          whitemech::lydia::dfa::logger.debug("variables: {}", join(variables));
        } else if (strfind(line, "states")) {
          fields = split(line, " +");
          nb_states = stoi(fields[1]);
          whitemech::lydia::dfa::logger.debug("number of states: {}",
                                              nb_states);
        } else if (strfind(line, "initial")) {
          fields = split(line, " +");
          initial_state = stoi(fields[1]);
          whitemech::lydia::dfa::logger.debug("initial state: {}",
                                              initial_state);
        } else if (strfind(line, "bdd nodes")) {
          fields = split(line, " ");
          nb_nodes = stoi(fields[2]);
          whitemech::lydia::dfa::logger.debug("nb nodes: {}", nb_nodes);
        } else if (strfind(line, "final")) {
          fields = split(line, " +");
          int i = 1; // start at 1 to ignore "final" token
          while (i < fields.size()) {
            if (fields[i] == "1")
              final_states.push_back(i - 1);
            i = i + 1;
          }
          whitemech::lydia::dfa::logger.debug("final states: {}",
                                              print_vect_int(final_states));
        } else if (strfind(line, "behaviour")) {
          fields = split(line, " +");
          int i = 1;
          while (i < fields.size()) {
            behaviour.push_back(stoi(fields[i]));
            i = i + 1;
          }
          whitemech::lydia::dfa::logger.debug("behaviour: {}",
                                              print_vect_int(behaviour));
        } else if (strfind(line, "bdd:"))
          flag = 1;
        else
          continue;
      } else {
        if (strfind(line, "end"))
          break;
        fields = split(line, " +");
        for (int i = 1; i < fields.size(); i++)
          tmp.push_back(stoi(fields[i]));
        mona_bdd_nodes.push_back(tmp);
        tmp.clear();
      }
    }
  } else {
    throw std::runtime_error("No such file or directory: " + filename);
  }
  f.close();

  // consistency checks
  assert(nb_variables > 0);
  assert(nb_states > 0);
  assert(initial_state >= 0 && initial_state < nb_states);
  for (auto final_state : final_states)
    assert(final_state >= 0 && final_state < nb_states);
  assert(mona_bdd_nodes.size() == nb_nodes);

  if (mgr == nullptr) {
    return new dfa(variables, nb_states, initial_state, final_states, behaviour,
                   mona_bdd_nodes);
  } else {
    return new dfa(mgr, variables, nb_states, initial_state, final_states,
                   behaviour, mona_bdd_nodes);
  }
}

dfa::dfa(CUDD::Cudd *mgr, const std::vector<std::string> &variables,
         int nb_states, int initial_state, const std::vector<int> &final_states,
         const std::vector<int> &behaviour, std::vector<item> &mona_bdd_nodes)
    : mgr{mgr} {
  this->nb_variables = variables.size();
  this->nb_states = nb_states;
  this->nb_bits = state2bin(nb_states - 1).length();
  this->initial_state = initial_state;
  this->variables = variables;
  construct_bdd_from_mona(mona_bdd_nodes, behaviour, final_states);
}

bool dfa::accepts(std::vector<interpretation> &word) {
  //  we preallocate the vector for performance purposes
  std::vector<int> extended_symbol = std::vector<int>(nb_bits + nb_variables);
  std::vector<int> next_state = std::vector<int>(nb_bits);
  std::vector<int> current_state = std::vector<int>(nb_bits);

  //  set next state to initial state
  auto initial_state_bits = state2bin(initial_state, nb_bits);
  for (int i = 0; i < nb_bits; i++)
    next_state[i] = (int)(initial_state_bits[i] == '1');

  for (auto symbol : word) {
    // populate extended symbol
    current_state = next_state;
    auto offset = nb_bits;
    // set state bits part
    for (int i = 0; i < nb_bits; i++)
      extended_symbol[i] = current_state[i];
    // set symbol part
    for (int i = 0; i < nb_variables; i++)
      extended_symbol[offset + i] = symbol[i];

    // compute next state
    int *extended_symbol_data = extended_symbol.data();
    for (int i = 0; i < root_bdds.size(); i++) {
      next_state[i] = (int)root_bdds[i].Eval(extended_symbol_data).IsOne();
    }
  }
  return finalstatesBDD.Eval(next_state.data()).IsOne();
}

int dfa::add_state() {
  int new_state = nb_states;
  auto new_nb_bits = bit_length(new_state);
  if (nb_bits < new_nb_bits) {
    // add new state variable
    CUDD::BDD b = mgr->bddVar();
    bddvars.insert(bddvars.begin() + nb_bits, b);
    ++nb_bits;
  }
  ++nb_states;
  return new_state;
}

void dfa::add_transition(int from, const interpretation &symbol, int to) {
  if (from < nb_states)
    throw std::invalid_argument("'from' state is not in the set of states.");
  if (to < nb_states)
    throw std::invalid_argument("'to' state is not in the set of states.");
}

} // namespace lydia
} // namespace whitemech
