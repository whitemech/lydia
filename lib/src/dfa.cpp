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

void dfa::recur(int index, item tmp) {
  if (smtbdd[index][0] == -1) {
    while (tmp.size() < nb_bits + nb_variables)
      tmp.push_back(2); // 0:false 1:true 2:don't care
    push_states(smtbdd[index][1], tmp);
    bdd.push_back(tmp);
    // print_vec(bdd);
    tmp.clear();
  } else {
    int left = smtbdd[index][1];
    int right = smtbdd[index][2];
    int v = smtbdd[index][0];
    recur_left(left, tmp, v);
    recur_right(right, tmp, v);
  }
}

void dfa::recur_left(int index, item tmp, int v) {
  while (tmp.size() < (nb_bits + v))
    tmp.push_back(2); // 0:false 1:true 2:don't care
  tmp.push_back(0);
  recur(index, tmp);
}

void dfa::recur_right(int index, item tmp, int v) {
  while (tmp.size() < (nb_bits + v)) //
    tmp.push_back(2);                // 0:false 1:true 2:don't care
  tmp.push_back(1);
  recur(index, tmp);
}

void dfa::get_bdd() {
  for (int i = 0; i < nb_states; i++) {
    int index = behaviour[i];
    item tmp;
    push_states(i, tmp);
    recur(index, tmp);
  }
}

void dfa::push_states(int i, item &tmp) {
  std::string s = state2bin(i);
  for (int j = 0; j < nb_bits - s.length(); j++)
    tmp.push_back(0);
  for (int j = 0; j < s.length(); j++) {
    int t = int(s[j]) - 48;
    tmp.push_back(t);
  }
}

void dfa::print_vec(std::vector<item> &v) {
  for (size_t n = 0; n < v.size(); n++)
    print_int(v[n]);
  std::cout << std::endl;
}

void dfa::bdd2dot() {
  for (int i = 0; i < res.size(); i++) {
    std::string filename = std::to_string(i);
    dumpdot(res[i], filename);
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

void dfa::construct_bdd() {
  for (int i = 0; i < nb_bits + nb_variables; i++) {
    CUDD::BDD b = mgr->bddVar();
    bddvars.push_back(b);
  }
  for (int i = 0; i < nb_bits; i++) {
    CUDD::BDD d = mgr->bddZero();
    res.push_back(d);
  }
  tBDD.resize(smtbdd.size());
  for (int i = 0; i < tBDD.size(); i++) {
    if (tBDD[i].size() == 0) {
      // dumpdot(tBDD[i][0], "test");
      vbdd b = try_get(i);
    }
  }

  for (int i = 0; i < nb_bits; i++) {
    for (int j = 0; j < nb_states; j++) {
      CUDD::BDD tmp = mgr->bddOne();
      std::string bins = state2bin(j);
      int offset = nb_bits - bins.size();
      for (int m = 0; m < offset; m++) {
        tmp = tmp * var2bddvar(0, m);
      }
      for (int m = 0; m < bins.size(); m++) {
        tmp = tmp * var2bddvar(int(bins[m]) - 48, m + offset);
      }
      tmp = tmp * tBDD[behaviour[j]][i];
      res[i] = res[i] + tmp;
    }
  }

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

vbdd dfa::try_get(int index) {
  if (tBDD[index].size() != 0)
    return tBDD[index];
  vbdd b;
  if (smtbdd[index][0] == -1) {
    int s = smtbdd[index][1];
    std::string bins = state2bin(s);
    for (int m = 0; m < nb_bits - bins.size(); m++) {
      CUDD::BDD temp_bdd = mgr->bddZero();
      b.push_back(temp_bdd);
    }
    for (int i = 0; i < bins.size(); i++) {
      if (bins[i] == '0') {
        CUDD::BDD temp_bdd = mgr->bddZero();
        b.push_back(temp_bdd);
      } else if (bins[i] == '1') {
        CUDD::BDD temp_bdd = mgr->bddOne();
        b.push_back(temp_bdd);
      } else
        std::cout << "error binary state" << std::endl;
    }
    tBDD[index] = b;
    return b;
  } else {
    int rootindex = smtbdd[index][0];
    int leftindex = smtbdd[index][1];
    int rightindex = smtbdd[index][2];
    CUDD::BDD root = bddvars[rootindex + nb_bits];
    // dumpdot(root, "test");
    vbdd left = try_get(leftindex);
    // for(int l = 0; l < left.size(); l++)
    // dumpdot(left[l], "left"+to_string(l));
    vbdd right = try_get(rightindex);
    // for(int l = 0; l < left.size(); l++)
    // dumpdot(right[l], "right"+to_string(l));
    assert(left.size() == right.size());
    for (int i = 0; i < left.size(); i++) {
      CUDD::BDD tmp = root.Ite(right[i], left[i]); // Assume this is correct
      // dumpdot(tmp, "tmp");
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

dfa *dfa::read_from_file(std::string filename) {
  int nb_variables = -1;
  std::vector<std::string> variables;
  int nb_states = -1;
  int initial_state = -1;
  int nb_nodes = -1;
  std::vector<item> smtbdd;
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
          // std::cout<<nb_variables<<std::endl;
        }
        if (strfind(line, "variables") && !strfind(line, "number")) {
          variables = split(line, " +");

        } else if (strfind(line, "states")) {
          fields = split(line, " +");
          nb_states = stoi(fields[1]);
          // std::cout<<nstates<<std::endl;
        } else if (strfind(line, "initial")) {
          fields = split(line, " +");
          initial_state = stoi(fields[1]);
          // std::cout<<init<<std::endl;
        } else if (strfind(line, "bdd nodes")) {
          fields = split(line, " ");
          nb_nodes = stoi(fields[2]);
          // std::cout<<nodes<<std::endl;
        } else if (strfind(line, "final")) {
          fields = split(line, " +");
          int i = 1; // start at 1 to ignore "final" token
          while (i < fields.size()) {
            if (fields[i] == "1")
              final_states.push_back(i - 1);
            i = i + 1;
          }
          // print_int(finalstates);
        } else if (strfind(line, "behaviour")) {
          fields = split(line, " +");
          int i = 1;
          while (i < fields.size()) {
            behaviour.push_back(stoi(fields[i]));
            i = i + 1;
          }
          // print_int(behaviour);
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
        smtbdd.push_back(tmp);
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
  assert(smtbdd.size() == nb_nodes);

  auto new_dfa = new dfa(nb_variables, nb_states, initial_state, final_states,
                         behaviour, smtbdd);
  return new_dfa;
}

dfa::dfa(CUDD::Cudd *mgr, int nb_variables, int nb_states, int initial_state,
         std::vector<int> final_states, std::vector<int> behaviour,
         std::vector<item> smtbdd)
    : mgr{mgr} {
  this->nb_variables = nb_variables;
  this->nb_states = nb_states;
  this->nb_bits = state2bin(nb_states - 1).length();
  this->initial_state = initial_state;
  this->final_states = std::move(final_states);
  this->behaviour = std::move(behaviour);
  this->smtbdd = std::move(smtbdd);
  this->nb_nodes = this->smtbdd.size();
  construct_bdd();
}

} // namespace lydia
} // namespace whitemech
