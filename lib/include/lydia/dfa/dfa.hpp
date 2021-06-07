#pragma once
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

// TODO require CUDD headers are installed in cudd/
#include <algorithm>
#include <cassert>
#include <cuddObj.hh>
#include <fstream>
#include <iostream>
#include <lydia/dfa/abstract_dfa.hpp>
#include <lydia/logger.hpp>
#include <lydia/types.hpp>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace whitemech::lydia {

class dfa : public abstract_dfa {
private:
  int initial_state{};
  int nb_variables{};
  int nb_states{};

public:
  int nb_bits{};

  std::vector<std::string> variables;

  CUDD::BDD finalstatesBDD;
  /*!
   * Store the BDD roots, from the least to the most significant:
   * b_0, b_1, ..., b_{n-1}
   */
  vec_bdd root_bdds;

  /*!
   * Store the BDD variables:
   * b_0, b_1, ..., b_{n-1}, var_0, var_1, ... var_{m-1}
   */
  vec_bdd bddvars;

  const CUDD::Cudd& mgr;

  dfa(const dfa&) = delete;
  dfa& operator=(const dfa&) = delete;
  dfa(dfa&&) = delete;
  dfa& operator=(dfa&&) = delete;

  /*!
   * Initialize the DFA from scratch.
   *
   * It creates one non-accepting state with a self-loop.
   * This is the default sink. We suggest to don't create
   * new transitions from it, otherwise it won't be a true sink anymore.
   *
   * Instead, do like the following:
   *
   * auto my_dfa = new dfa::dfa(...);
   * my_dfa.add_state();
   * my_dfa.set_initial_state(1);
   * // add transitions from 1...
   *
   * The DFA can be later extended by using the methods like:
   * - add_state
   * - add_transition
   * - set_initial_state
   * - set_final_state
   *
   * @param mgr the CUDD manager.
   * @param nb_bits the maximum number of bits available.
   * @param nb_variables the number of variables to be used.
   */
  dfa(const CUDD::Cudd& mgr, int nb_bits, int nb_variables);

  /*
   *
   * Constructor from MONA data.
   *
   * It is not suggested to use this constructor directly,
   * but through the static function dfa::read_from_file.
   *
   * It will build the DFA using the technique
   * explained in Section 5.1 of [1].
   *
   * [1] Shufang et. al. 2017. Symbolic LTLf Synthesis.
   * https://arxiv.org/abs/1705.08426
   *
   * @param mgr the CUDD manager
   * @param nb_variables the number of variables
   * @param nb_states the number of states
   * @param initial_state the initial state
   * @param final_states the final state
   * @param behaviour the MONA behaviours
   * @param mona_bdd_nodes the shared multi-terminal BDD nodes (MONA DFA)
   */
  dfa(const CUDD::Cudd& mgr, const std::vector<std::string>& variables,
      int nb_states, int initial_state, const std::vector<int>& final_states,
      const std::vector<int>& behaviour, std::vector<item>& mona_bdd_nodes);

  static Logger logger;

  // void initialize(string filename, string partfile, Cudd& manager);
  std::vector<item> bdd;

  CUDD::BDD state2bdd(int s);

  /*!
   *
   * Parse a MONA DFA file.
   *
   * Please check Appendix C of the MONA User Manual [1].
   *
   * [1] MONA User Manual. https://www.brics.dk/mona/mona14.pdf
   *
   * @param filename path to the MONA DFA file.
   * @return a raw pointer to a DFA.
   */
  static dfa read_from_file(const std::string& filename, const CUDD::Cudd& mgr);

  /*!
   * Check whether a word of propositional interpretations
   * is accepted by the DFA.
   *
   * //TODO consider using bit vectors for an interpretation.
   *
   * @return true if the word is accepted, false otherwise.
   */
  bool accepts(const trace& word) const override;

  int get_initial_state() const override { return initial_state; };
  int get_nb_states() const override { return nb_states; };
  int get_nb_variables() const override { return nb_variables; };
  int get_successor(int state, const interpretation& symbol) const override;
  int get_successor(int state, const interpretation_set& symbol) const override;

  CUDD::BDD get_symbol(const interpretation_map&) const;

  bool is_final(int state) const override;

  /*!
   * Add a new state.
   *
   * @return the index of the next state.
   */
  int add_state() override;

  /*!
   * Set the initial state.
   *
   * @param state the initial state.
   */
  void set_initial_state(int state) override;

  /*!
   * Set a state to be final (or not final)
   *
   * @param state the initial state.
   * @param is_final whether the state should be final or not..
   */
  void set_final_state(int state, bool is_final = true) override;

  /*!
   * Add a transition to the DFA.
   *
   * The @from and @to parameters must be already existing states
   * of the DFA. The variable indexes (i.e. the keys of @symbols)
   * that are greater than nb_variables are ignored.
   *
   * @param from the starting DFA state
   * @param symbol the guard of the transition. That is, a mapping from indexes
   *    associated to variables, and value of truth of that variable.
   *    Variables indexes missing from the set of keys, or indexes
   *    greater or equal than nb_variables, are interpreted as "don't care".
   * @param to the ending DFA state
   */
  void add_transition(int from, const interpretation_map& symbol,
                      int to) override;

  /*!
   * The same the above, but with @symbol as a vector of indexes whose
   * variables are considered true.
   *
   * If @dont_care is true (default), the missing variables are considered
   * "don't care" variables. Otherwise, they are considered explicitly false.
   */
  void add_transition(int from, const interpretation& symbol, int to,
                      bool dont_care = true) override;

  /*!
   * The same the above, but with @symbol as a set of indexes whose
   * variables are considered true.
   *
   * If @dont_care is true (default), the missing variables are considered
   * "don't care" variables. Otherwise, they are considered explicitly false.
   */
  void add_transition(int from, const interpretation_set& symbol, int to,
                      bool dont_care = true) override;

  CUDD::BDD prop2bddvar(int index, bool v) const;

protected:
private:
  void get_successor(const std::vector<int>& state,
                     const interpretation& symbol, std::vector<int>& next_state,
                     std::vector<int>& extended_symbol) const;

  /*!
   * Given the index, try to get a BDD. If not present yet, create it.
   *
   * If it is a terminal, then instantiate it by using the binary
   * representation of the terminal state (remember, in a shared
   * multi-terminal BDD the terminal nodes have an integer that correspond
   * to the successor).
   *
   * If it is a variable, then build the ITE node (for each bit)
   *
   * @param index the index of the BDD
   * @param mona_bdd_nodes
   * @return the list of BDDs.
   */
  vec_bdd try_get(int index,
                  const std::vector<std::vector<int>>& mona_bdd_nodes,
                  std::vector<vec_bdd>& tBDD);

  /*!
   * Return positive or negative BDD variable from index.
   *
   * @param index an integer between 0 and (nb_bits + nb_variables)
   * @param v a boolean to say whether the variable should be positive or
   * negative.
   * @return the BDD variable corresponding to the index and its
   */
  CUDD::BDD var2bddvar(int index, bool v = true) const;

  /*!
   * This method builds the Symbolic DFA from MONA BDD nodes.
   *
   * The input is a list of BDD specifications.
   * A BDD specification is a triple of integers to be interpreted as follows:
   * - if the ith element contains "{-1, val, 0}", the ith BDD node is a leaf
   * with value val.
   * - if the ith element contains "{x, l, r}", the ith BDD node is an internal
   * node with index x, left (low) successor l and right (high) successor r.
   *
   * [1] MONA User Manual. https://www.brics.dk/mona/mona14.pdf
   *
   * @param mona_bdd_nodes MONA BDDs specifications.
   */
  void
  construct_bdd_from_mona(const std::vector<std::vector<int>>& mona_bdd_nodes,
                          const std::vector<int>& behaviour,
                          const std::vector<int>& final_states);
};
} // namespace whitemech::lydia
