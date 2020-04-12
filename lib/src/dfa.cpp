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
#include <utils/strings.hpp>

namespace whitemech {
namespace lydia {

Logger dfa::logger = Logger("dfa");

// void dfa::construct_from_comp_back(vbdd &S2S, vbdd &S2P, vbdd &Svars,
//                                   vbdd &Ivars, vbdd &Ovars,
//                                   std::vector<int> IS) {
//
//  // substitute P from res, first create a substitution/projection vector,
//  then
//  // use the batch substitution function
//  vbdd_ptr subnProj;
//  // task dfa states
//  for (int i = 0; i < nb_bits; i++) {
//    subnProj.push_back(bddvars[i]);
//  }
//  // propositions (aka task variables)
//  assert(S2P.size() == nb_variables);
//  for (int i = 0; i < nb_variables; i++) {
//    // TODO: We need to make sure the variables line up!!!!
//    subnProj.push_back(S2P[i]);
//  }
//
//  for (int i = 0; i < res.size(); i++) {
//    res[i] = res[i].VectorCompose(subnProj);
//  }
//
//  // append the propositions to res
//  res.insert(res.end(), S2P.begin(), S2P.end());
//  // append S2S to res
//  res.insert(res.end(), S2S.begin(), S2S.end());
//
//  // fix the other variables (nb_variables, nbits, init, etc)
//  // std::cout<<"constructing bdd with
//  // "<<bddvars.size()<<"variables"<<std::endl;
//  bddvars.insert(bddvars.end(), Svars.begin(), Svars.end());
//  bddvars.insert(bddvars.end(), Ivars.begin(), Ivars.end());
//  bddvars.insert(bddvars.end(), Ovars.begin(), Ovars.end());
//  // std::cout<<"constructing bdd with
//  // "<<bddvars.size()<<"variables"<<std::endl;
//  // make init bitvector (final states is a bdd, does not need change)
//  initbv = new int[nb_bits + nb_variables + Svars.size()];
//  int temp = initial_state;
//  for (int i = nb_bits - 1; i >= 0; i--) {
//    initbv[i] = temp % 2;
//    temp = temp / 2;
//  }
//  for (int i = 0; i < nb_variables; i++) {
//    initbv[i + nb_bits] = 0;
//  }
//  for (int i = 0; i < IS.size(); i++) {
//    initbv[i + nb_bits + nb_variables] = IS[i];
//  }
//    nb_bits = nb_bits + nb_variables;
//}

void dfa::push_states(int i, item &tmp) {
  std::string s = state2bin(i);
  for (int j = 0; j < nb_bits - s.length(); j++)
    tmp.push_back(0);
  for (int j = 0; j < s.length(); j++) {
    int t = int(s[j]) - 48;
    tmp.push_back(t);
  }
}

void dfa::bdd2dot(std::string directory) {
  for (int i = 0; i < root_bdds.size(); i++) {
    std::string filename = directory + std::to_string(i);
    dumpdot(root_bdds[i], filename);
  }
}

// return positive or nagative bdd variable index
CUDD::BDD dfa::var2bddvar(int v, int index) {
  if (v == 0) {
    return !bddvars[index];
  } else {
    return bddvars[index];
  }
}

void dfa::construct_bdd_from_mona(
    std::vector<std::vector<int>> mona_bdd_nodes) {
  // Create all the variables, the ones for the bits of the states and the ones
  // for the variables.
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
  tBDD.resize(mona_bdd_nodes.size());
  for (int i = 0; i < tBDD.size(); i++) {
    if (tBDD[i].size() == 0) {
      vbdd b = try_get(i, mona_bdd_nodes);
    }
  }
  // Assign each state to the next BDD node.
  // Basically, parse the content of the MONA DFA 'behaviour' vector.
  for (int i = 0; i < nb_bits; i++) {
    for (int j = 0; j < nb_states; j++) {
      //    Build the BDD representation of a state.
      //    we set the temporary variable to 1
      //    because we are going to put it in 'AND'.
      CUDD::BDD tmp = mgr->bddOne();
      std::string bins = state2bin(j);
      int offset = nb_bits - bins.size();
      for (int m = 0; m < offset; m++) {
        tmp = tmp * var2bddvar(0, m);
      }
      for (int m = 0; m < bins.size(); m++) {
        auto bit_mth = int(bins[m]) - 48;
        tmp = tmp * var2bddvar(bit_mth, offset + m);
      }
      //    Now, put in 'AND' with the right
      //    BDD node (either terminal/leaf or variable)
      //    At the bit 'i'.
      tmp = tmp * tBDD[behaviour[j]][i];
      //    Put each root bdd in OR with the current temporary BDD.
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

/*!
 * TODO elaborate
 * Given the index, try to get a BDD. If not present yet, create it.
 *
 * If it is a terminal, then instantiate it by using the binary
 * representation of the terminal state (remember, in a shared
 * multi-terminal BDD the terminal nodes have an integer that correspond
 * to the successor).
 *
 * If it is a variable, then build the ITE node.
 *
 * @param index the index of the BDD
 * @return the list of BDDs.
 */
vbdd dfa::try_get(int index, std::vector<std::vector<int>> &mona_bdd_nodes) {
  if (tBDD[index].size() != 0)
    return tBDD[index];
  vbdd b;
  if (mona_bdd_nodes[index][0] == -1) {
    //     case when BDD node is a leaf
    //     the format is: "-1 val 0"
    int value = mona_bdd_nodes[index][1];
    //     get the binary representation of the state
    std::string bins = state2bin(value);
    //    this representation may use less bits
    //    than all the bits pre-allocated.
    //    Set to zero the bits in excess.
    for (int m = 0; m < nb_bits - bins.size(); m++) {
      CUDD::BDD temp_bdd = mgr->bddZero();
      b.push_back(temp_bdd);
    }
    //     Populate the LSBs
    for (int i = 0; i < bins.size(); i++) {
      if (bins[i] == '0') {
        CUDD::BDD temp_bdd = mgr->bddZero();
        b.push_back(temp_bdd);
      } else if (bins[i] == '1') {
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
    vbdd low = try_get(leftindex, mona_bdd_nodes);
    vbdd high = try_get(rightindex, mona_bdd_nodes);
    assert(low.size() == high.size());
    for (int i = 0; i < low.size(); i++) {
      // TODO old comment said "Assuming this is correct". Investigate.
      CUDD::BDD tmp = root.Ite(high[i], low[i]);
      b.push_back(tmp);
    }
    tBDD[index] = b;
    return b;
  }
}

void dfa::dumpdot(CUDD::BDD &b, std::string filename) {
  FILE *fp = fopen(filename.c_str(), "w");
  std::vector<CUDD::BDD> single(1);
  single[0] = b;
  this->mgr->DumpDot(single, NULL, NULL, fp);
  fclose(fp);
}

dfa *dfa::read_from_file(std::string filename, CUDD::Cudd *mgr) {
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
          std::cout << nb_variables << std::endl;
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
    return new dfa(nb_variables, nb_states, initial_state, final_states,
                   behaviour, mona_bdd_nodes);
  } else {
    return new dfa(mgr, nb_variables, nb_states, initial_state, final_states,
                   behaviour, mona_bdd_nodes);
  }
}

dfa::dfa(CUDD::Cudd *mgr, int nb_variables, int nb_states, int initial_state,
         std::vector<int> final_states, std::vector<int> behaviour,
         std::vector<item> &mona_bdd_nodes)
    : mgr{mgr} {
  this->nb_variables = nb_variables;
  this->nb_states = nb_states;
  this->nb_bits = state2bin(nb_states - 1).length();
  this->initial_state = initial_state;
  this->final_states = std::move(final_states);
  this->behaviour = std::move(behaviour);
  construct_bdd_from_mona(mona_bdd_nodes);
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

} // namespace lydia
} // namespace whitemech
