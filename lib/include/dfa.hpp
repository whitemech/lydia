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
  dfa(CUDD::Cudd *mgr, int nb_variables, int nb_states, int initial_state,
      std::vector<int> final_states, std::vector<int> behaviour,
      std::vector<item> &mona_bdd_nodes);

  /*
   * The same constructor as above, but the manager
   * will be instantiated in the constructor.
   */
  dfa(int nb_variables, int nb_states, int initial_state,
      std::vector<int> final_states, std::vector<int> behaviour,
      std::vector<item> &smtbdd)
      : dfa(new CUDD::Cudd(), nb_variables, nb_states, initial_state,
            std::move(final_states), std::move(behaviour), smtbdd) {}

  static Logger logger;

  // void initialize(string filename, string partfile, Cudd& manager);
  std::vector<item> bdd;
  void bdd2dot(std::string directory = "./");
  void dumpdot(CUDD::BDD &b, std::string filename);
  CUDD::BDD state2bdd(int s);
  int nb_bits;
  int initial_state;
  int nb_states;
  int *initbv;

  int nb_variables;
  std::vector<int> final_states;
  CUDD::BDD finalstatesBDD;
  //  Store the BDD roots - LSB order
  vbdd root_bdds;
  vbdd bddvars;

  std::vector<std::string> variables;

  const std::unique_ptr<CUDD::Cudd> mgr;

  // domain-spec separate construction
  // Front need to be called before variable construction for domain
  // back is called after the components are constructed
  void construct_from_comp_back(vbdd &S2S, vbdd &S2P, vbdd &Svars, vbdd &Ivars,
                                vbdd &Ovars, std::vector<int> IS);

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
  static dfa *read_from_file(std::string filename, CUDD::Cudd *mgr = nullptr);

  /*!
   * Check whether a word of propositional interpretations
   * is accepted by the DFA.
   *
   * @return true if the word is accepted, false otherwise.
   */
  bool accepts(std::vector<interpretation> &word);

protected:
private:
  std::vector<int> behaviour;
  //		vector<string> variables;
  void push_states(int i, item &tmp);
  CUDD::BDD var2bddvar(int v, int index);

  // new bdd constructer
  std::vector<vbdd> tBDD;
  vbdd try_get(int index, std::vector<std::vector<int>> &mona_bdd_nodes);
  void construct_bdd_from_mona(std::vector<std::vector<int>> mona_bdd_nodes);
};

} // namespace lydia
} // namespace whitemech
