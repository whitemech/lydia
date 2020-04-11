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
#include "types.hpp"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace whitemech {
namespace lydia {

class dfa {
public:
  explicit dfa(CUDD::Cudd *m) : mgr{m} {};
  dfa(int nb_variables, int nb_states, int initial_state,
      std::vector<int> final_states, std::vector<int> behaviour,
      std::vector<item> smtbdd);

  ~dfa() {
    // dtor
    delete mgr; // = NULL;
  }
  // void initialize(string filename, string partfile, Cudd& manager);
  std::vector<item> bdd;
  void print_vec(std::vector<item> &v);
  void bdd2dot();
  void dumpdot(CUDD::BDD &b, std::string filename);
  CUDD::BDD state2bdd(int s);
  int nb_bits;
  int initial_state;
  int *initbv;
  int nb_states;

  int nb_variables;
  std::vector<int> final_states;
  std::vector<item> smtbdd;
  CUDD::BDD finalstatesBDD;
  std::vector<CUDD::BDD> res;
  std::vector<CUDD::BDD> bddvars;
  std::vector<int> input;
  std::vector<int> output;

  std::vector<std::string> variables;

  const CUDD::Cudd *mgr;

  // domain-spec separate construction
  // Front need to be called before variable construction for domain
  // back is called after the components are constructed
  void construct_from_comp_back(vbdd &S2S, vbdd &S2P, vbdd &Svars, vbdd &Ivars,
                                vbdd &Ovars, std::vector<int> IS);

  static dfa *read_from_file(std::string filename);

protected:
private:
  int nodes;
  std::vector<int> behaviour;
  //		vector<string> variables;
  void get_bdd();
  void recur(int index, item tmp);
  void recur_left(int index, item tmp, int v);
  void recur_right(int index, item tmp, int v);
  void push_states(int i, item &tmp);
  CUDD::BDD var2bddvar(int v, int index);

  // new bdd constructer
  std::vector<vbdd> tBDD;
  vbdd try_get(int index);
  void construct_bdd();
};

} // namespace lydia
} // namespace whitemech
