#pragma once
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

#include "cuddObj.hh"
#include "logger.hpp"
#include "types.hpp"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace whitemech {
namespace lydia {

class dfa {
public:
  explicit dfa(std::unique_ptr<CUDD::Cudd> &m) : mgr{std::move(m)} {};

  dfa(const dfa &) = delete;
  dfa &operator=(const dfa &) = delete;
  dfa(dfa &&) = delete;
  dfa &operator=(dfa &&) = delete;

  dfa(CUDD::Cudd *mgr, int nb_variables);

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
  dfa(CUDD::Cudd *mgr, const std::vector<std::string> &variables, int nb_states,
      int initial_state, const std::vector<int> &final_states,
      const std::vector<int> &behaviour, std::vector<item> &mona_bdd_nodes);

  /*
   * The same constructor as above, but the manager
   * will be instantiated in the constructor.
   */
  dfa(const std::vector<std::string> &variables, int nb_states,
      int initial_state, const std::vector<int> &final_states,
      const std::vector<int> &behaviour, std::vector<item> &smtbdd)
      : dfa(new CUDD::Cudd(), variables, nb_states, initial_state, final_states,
            behaviour, smtbdd) {}

  static Logger logger;

  // void initialize(string filename, string partfile, Cudd& manager);
  std::vector<item> bdd;

  /*!
   * Dump the BDDs (one for each bit of the state space).
   *
   * The output will be:
   * output_directory/
   * - 0.dot
   * - 1.dot
   * ...
   * - n.dot
   *
   * That is, a DOT file for each bit.
   *
   * @param directory the directory in which to print the BDDs in DOT format.
   */
  void bdd2dot(const std::string &directory = "./");
  void dumpdot(CUDD::BDD &b, const std::string &filename);
  CUDD::BDD state2bdd(int s);
  int nb_bits{};
  int initial_state{};
  int nb_states{};
  int nb_variables{};

  CUDD::BDD finalstatesBDD;
  //  Store the BDD roots - LSB order
  vec_bdd root_bdds;
  vec_bdd bddvars;

  const std::unique_ptr<CUDD::Cudd> mgr;

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
  static dfa *read_from_file(const std::string &filename,
                             CUDD::Cudd *mgr = nullptr);

  /*!
   * Check whether a word of propositional interpretations
   * is accepted by the DFA.
   *
   * //TODO consider using bit vectors for an interpretation.
   *
   * @return true if the word is accepted, false otherwise.
   */
  bool accepts(std::vector<interpretation> &word);

  /*!
   * Add a new state.
   *
   * @return the index of the next state.
   */
  int add_state();

  /*!
   * Set the initial state.
   *
   * @param state the initial state.
   */
  void set_initial_state(int state);

  /*!
   * Add a transition to the DFA.
   *
   * The @from and @to parameters must be already existing states
   * of the DFA.
   *
   * We assume the variables in the interpretation are all present
   * in the DFA.
   *
   * @param from the starting DFA state
   * @param symbol the guard of the transition
   * @param to the ending DFA state
   */
  void add_transition(int from, const interpretation &symbol, int to);

protected:
private:
  std::vector<std::string> variables;

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
                  const std::vector<std::vector<int>> &mona_bdd_nodes,
                  std::vector<vec_bdd> &tBDD);

  CUDD::BDD var2bddvar(int v, int index);

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
  construct_bdd_from_mona(const std::vector<std::vector<int>> &mona_bdd_nodes,
                          const std::vector<int> &behaviour,
                          const std::vector<int> &final_states);
};
} // namespace lydia
} // namespace whitemech
