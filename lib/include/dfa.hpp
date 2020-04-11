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
  explicit dfa(CUDD::Cudd *m);
  dfa();
  virtual ~dfa();
  void initialize(std::string filename, std::string partfile);
  // void initialize(string filename, string partfile, Cudd& manager);
  std::vector<item> bdd;
  void print_vec(std::vector<item> &v);
  void construct_bdd();
  void bdd2dot();
  void dumpdot(CUDD::BDD &b, std::string filename);
  CUDD::BDD state2bdd(int s);
  int nbits;
  int init;
  int *initbv;
  int nstates;

  int nvars;
  std::vector<int> finalstates;
  CUDD::BDD finalstatesBDD;
  std::vector<CUDD::BDD> res;
  std::vector<CUDD::BDD> bddvars;
  std::vector<int> input;
  std::vector<int> output;

  std::vector<std::string> variables;

  // new bdd constructer
  void construct_bdd_new();

  CUDD::Cudd *mgr;

  // domain-spec separate construction
  // Front need to be called before variable construction for domain
  // back is called after the components are constructed
  void construct_from_comp_front(std::string filename);
  void construct_from_comp_back(vbdd &S2S, vbdd &S2P, vbdd &Svars, vbdd &Ivars,
                                vbdd &Ovars, std::vector<int> IS);

protected:
private:
  int nodes;
  std::vector<int> behaviour;
  //		vector<string> variables;
  std::vector<item> smtbdd;
  void read_from_file(std::string filename); // read the ltlf formula
  void read_partfile(std::string partfile);  // read the partfile
  void get_bdd();
  void recur(int index, item tmp);
  void recur_left(int index, item tmp, int v);
  void recur_right(int index, item tmp, int v);
  void print(std::vector<std::string> &v);
  void print_int(std::vector<int> &v);
  bool strfind(std::string str, std::string target);
  void push_states(int i, item &tmp);
  std::string state2bin(int n);
  CUDD::BDD var2bddvar(int v, int index);

  // new bdd constructer
  std::vector<vbdd> tBDD;
  vbdd try_get(int index);
};

} // namespace lydia
} // namespace whitemech
