/*
 * This file is part of Lydia.
 *
 * Lydia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Lydia is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lydia.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cuddObj.hh>
#include <lydia/dfa/dfa.hpp>
#include <lydia/utils/misc.hpp>
#include <lydia/utils/strings.hpp>
#include <numeric>

namespace whitemech::lydia {

Logger dfa::logger = Logger("dfa");

dfa::dfa(const CUDD::Cudd& mgr, int nb_bits, int nb_variables)
    : mgr{mgr}, nb_bits{nb_bits}, nb_states{1}, initial_state{0},
      nb_variables{nb_variables} {

  bddvars.reserve(nb_variables);
  root_bdds.reserve(nb_bits + nb_variables);
  variables.reserve(nb_variables);

  //  add BDD variables for all the bits and variables
  for (int i = 0; i < nb_bits + nb_variables; i++) {
    CUDD::BDD b = this->mgr.bddVar();
    bddvars.push_back(b);
  }

  for (int i = 0; i < nb_bits; i++) {
    // state 0 is a sink state - put a self-loop at bit 0.
    CUDD::BDD zero = this->mgr.bddZero();
    root_bdds.push_back(zero);
  }

  //  start with empty set of final states.
  finalstatesBDD = this->mgr.bddZero();

  // populate default names: p_0, p_1, ...
  std::vector<int> range(nb_variables);
  std::iota(range.begin(), range.end(), 0);
  std::transform(std::begin(range), std::end(range),
                 std::back_inserter(variables),
                 [](int i) { return "p_" + std::to_string(i); });
  for (int i = nb_bits; i < nb_variables; i++) {
    auto s = variables.at(i);
    mgr.pushVariableName(s);
  }
}

CUDD::BDD dfa::var2bddvar(int index, bool v) const {
  return v ? bddvars[index] : !bddvars[index];
}

CUDD::BDD dfa::prop2bddvar(int index, bool v) const {
  assert(index < nb_variables);
  return var2bddvar(nb_bits + index, v);
}

void dfa::construct_bdd_from_mona(
    const std::vector<std::vector<int>>& mona_bdd_nodes,
    const std::vector<int>& behaviour, const std::vector<int>& final_states) {
  // Create all the variables, the ones for the bits of the states and the ones
  // for the variables.
  auto tBDD = std::vector<vec_bdd>(mona_bdd_nodes.size());
  for (int i = 0; i < nb_bits + nb_variables; i++) {
    CUDD::BDD b = mgr.bddVar();
    bddvars.push_back(b);
  }
  // For each bit of the state, create a BDD zero constant
  // Each var will be updated later in 'OR' (that's why we put zero)
  for (int i = 0; i < nb_bits; i++) {
    CUDD::BDD d = mgr.bddZero();
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
      // we set the temporary variable to 1 because
      // we are going to put it in 'AND' later.
      CUDD::BDD tmp = mgr.bddOne();
      // the first for-loop handles the most significant bits
      // that are not covered by the binary representation.
      // the second for-loop handles all the other bits.
      std::string bins = state2bin(j, nb_bits, true);
      for (int m = 0; m < nb_bits; m++) {
        bool bit_mth = int(bins[m]) - '0';
        tmp = tmp * var2bddvar(m, bit_mth);
      }
      // Now, put in 'AND' with the correct BDD node
      // (either terminal/leaf or variable) at the bit i_th.
      tmp = tmp * tBDD[behaviour[j]][i];
      // Put each root bdd in OR with the current temporary BDD.
      root_bdds[i] = root_bdds[i] + tmp;
    }
  }

  // Create the BDD for recognizing the accepting states.
  finalstatesBDD = mgr.bddZero();
  for (int finalstate : final_states) {
    CUDD::BDD ac = state2bdd(finalstate);
    finalstatesBDD += ac;
  }
}

CUDD::BDD dfa::state2bdd(int s) {
  std::string bin = state2bin(s, nb_bits, true);
  CUDD::BDD b = mgr.bddOne();
  for (int j = 0; j < nb_bits; j++) {
    bool bit = int(bin[j]) - '0';
    b *= var2bddvar(j, bit);
  }
  return b;
}

vec_bdd dfa::try_get(int index,
                     const std::vector<std::vector<int>>& mona_bdd_nodes,
                     std::vector<vec_bdd>& tBDD) {
  if (!tBDD[index].empty())
    return tBDD[index];
  vec_bdd b;
  b.reserve(nb_bits);
  if (mona_bdd_nodes[index][0] == -1) {
    // case when BDD node is a leaf
    // the format is: "-1 val 0"
    int value = mona_bdd_nodes[index][1];
    // get the binary representation of the state
    std::string bins = state2bin(value, nb_bits, true);
    for (char bin : bins) {
      if (bin == '0') {
        CUDD::BDD temp_bdd = mgr.bddZero();
        b.push_back(temp_bdd);
      } else if (bin == '1') {
        CUDD::BDD temp_bdd = mgr.bddOne();
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

dfa dfa::read_from_file(const std::string& filename, const CUDD::Cudd& mgr) {
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

  return dfa(mgr, variables, nb_states, initial_state, final_states, behaviour,
             mona_bdd_nodes);
}

dfa::dfa(const CUDD::Cudd& mgr, const std::vector<std::string>& variables,
         int nb_states, int initial_state, const std::vector<int>& final_states,
         const std::vector<int>& behaviour, std::vector<item>& mona_bdd_nodes)
    : mgr{mgr} {
  this->nb_variables = variables.size();
  this->nb_states = nb_states;
  this->nb_bits = state2bin(nb_states - 1).length();
  this->initial_state = initial_state;
  this->variables = variables;
  construct_bdd_from_mona(mona_bdd_nodes, behaviour, final_states);
}

bool dfa::accepts(const trace& word) const {
  //  we preallocate the vector for performance purposes
  std::vector<int> extended_symbol = std::vector<int>(nb_bits + nb_variables);
  std::vector<int> next_state = std::vector<int>(nb_bits);
  std::vector<int> current_state = std::vector<int>(nb_bits);

  //  set next state to initial state
  auto initial_state_bits = state2bin(initial_state, nb_bits, true);
  for (int i = 0; i < nb_bits; i++)
    next_state[i] = (int)(initial_state_bits[i] == '1');

  // start the evaluation loop, until the end of the word
  for (const auto& symbol : word) {
    current_state = next_state;
    get_successor(current_state, symbol, next_state, extended_symbol);
  }
  return finalstatesBDD.Eval(next_state.data()).IsOne();
}

int dfa::add_state() {
  int new_state = nb_states;
  auto new_nb_bits = bit_length(new_state);
  if (nb_bits < new_nb_bits) {
    /* TODO discuss: that requires a variable number of bits!
     *      How to deal with final_states_BDD?
     *
     *    // add new state variable
     *    CUDD::BDD b = mgr.bddVar();
     *    bddvars.insert(bddvars.begin() + nb_bits, b);
     *    ++nb_bits;
     */
    throw std::invalid_argument(
        "Maximum number of states reached: Cannot instantiate a new bit.");
  }
  ++nb_states;
  return new_state;
}

void dfa::set_initial_state(int state) {
  if (state >= nb_states) {
    throw std::invalid_argument("The state is not in the set of states.");
  }
  this->initial_state = state;
}

void dfa::set_final_state(int state, bool is_final) {
  if (state >= nb_states) {
    throw std::invalid_argument("The state is not in the set of states.");
  }
  CUDD::BDD tmp = state2bdd(state);
  finalstatesBDD += (is_final ? tmp : !tmp);
}

void dfa::add_transition(int from, const interpretation_map& symbol, int to) {
  if (from >= nb_states)
    throw std::invalid_argument("'from' state is not in the set of states.");
  if (to >= nb_states)
    throw std::invalid_argument("'to' state is not in the set of states.");
  if (symbol.size() > nb_variables)
    throw std::invalid_argument("Too many variables!");
  //  TODO add some validation on symbol

  std::string to_binary = state2bin(to, nb_bits, true);

  // build the extended symbol in form of a BDD
  CUDD::BDD tmp = state2bdd(from);
  for (auto pair : symbol) {
    tmp = tmp * var2bddvar(nb_bits + pair.first, pair.second);
  }

  //  Update each root BDD.
  assert(nb_bits == to_binary.length());
  for (int i = 0; i < nb_bits; i++) {
    bool result = int(to_binary[i]) - '0';
    if (result)
      root_bdds[i] += tmp;
  }
}

void dfa::add_transition(int from, const interpretation& symbol, int to,
                         bool dont_care) {
  interpretation_set s{symbol.begin(), symbol.end()};
  add_transition(from, s, to, dont_care);
}

void dfa::add_transition(int from, const interpretation_set& symbol, int to,
                         bool dont_care) {
  std::map<int, bool> new_symbol;
  if (dont_care) {
    for (int i : symbol)
      new_symbol[i] = true;
  } else {
    for (int i = 0; i < nb_variables; i++)
      new_symbol[i] = symbol.find(i) != symbol.end();
  }

  add_transition(from, new_symbol, to);
}

int dfa::get_successor(int state, const interpretation& symbol) const {
  std::vector<int> current_state = std::vector<int>(nb_bits);
  std::vector<int> next_state(nb_bits);
  std::vector<int> extended_symbol(nb_bits + nb_variables);

  auto initial_state_bits = state2bin(state, nb_bits, true);
  for (int i = 0; i < nb_bits; i++)
    current_state[i] = (int)(initial_state_bits[i] == '1');

  get_successor(current_state, symbol, next_state, extended_symbol);

  int result = bin2state(next_state);
  return result;
}

void dfa::get_successor(const std::vector<int>& state,
                        const interpretation& symbol,
                        std::vector<int>& next_state,
                        std::vector<int>& extended_symbol) const {
  auto offset = nb_bits;
  // set state bits part
  for (int i = 0; i < nb_bits; i++)
    extended_symbol[i] = state[i];
  // set symbol part
  for (int i = 0; i < nb_variables; i++)
    extended_symbol[offset + i] = symbol[i];

  // compute next state
  int* extended_symbol_data = extended_symbol.data();
  for (int i = 0; i < root_bdds.size(); i++) {
    next_state[i] = (int)root_bdds[i].Eval(extended_symbol_data).IsOne();
  }
}

int dfa::get_successor(int state, const interpretation_set& symbol) const {
  interpretation vec_symbol(nb_variables, 0);
  for (const int& variable : symbol) {
    vec_symbol[variable] = 1;
  }
  return get_successor(state, vec_symbol);
}

bool dfa::is_final(int state) const {
  std::vector<int> state_as_binary_vect = state2binvec(state, nb_bits);
  return finalstatesBDD.Eval(state_as_binary_vect.data()).IsOne();
}

CUDD::BDD dfa::get_symbol(const interpretation_map& i) const {
  CUDD::BDD tmp = mgr.bddOne();
  for (const auto& pair : i) {
    tmp = tmp * var2bddvar(nb_bits + pair.first, pair.second);
  }
  return tmp;
}

} // namespace whitemech::lydia
