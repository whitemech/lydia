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

dfa::dfa(CUDD::Cudd *m) {
  mgr = m;
  // ctor
}

dfa::dfa() { mgr = new CUDD::Cudd(); }

dfa::~dfa() {
  // dtor
  delete mgr; // = NULL;
}
void dfa::initialize(std::string filename, std::string partfile) {
  // ctor
  read_from_file(filename);
  nbits = state2bin(nstates - 1).length();

  // get_bdd();
  // print_vec(bdd);
  // construct_bdd();
  construct_bdd_new();
  read_partfile(partfile);

  initbv = new int[nbits];
  int temp = init;
  for (int i = nbits - 1; i >= 0; i--) {
    initbv[i] = temp % 2;
    temp = temp / 2;
  }
}

void dfa::read_partfile(std::string partfile) {
  std::ifstream f(partfile.c_str());
  std::vector<std::string> inputs;
  std::vector<std::string> outputs;
  std::string line;
  while (getline(f, line)) {
    if (f.is_open()) {
      if (strfind(line, "inputs")) {
        inputs = split(line, " +");
        // print(inputs);
      } else if (strfind(line, "outputs")) {
        outputs = split(line, " +");
        // print(outputs);
      } else {
        std::cout << "read partfile error!" << std::endl;
        std::cout << partfile << std::endl;
        std::cout << line << std::endl;
      }
    }
  }
  f.close();
  std::set<std::string> input_set;
  std::set<std::string> output_set;
  for (int i = 1; i < inputs.size(); i++) {
    std::string c = to_upper_case(inputs[i]);
    input_set.insert(c);
  }
  for (int i = 1; i < outputs.size(); i++) {
    std::string c = to_upper_case(outputs[i]);
    output_set.insert(c);
  }

  for (int i = 1; i < variables.size(); i++) {
    if (input_set.find(variables[i]) != input_set.end())
      input.push_back(nbits + i - 1);
    else if (output_set.find(variables[i]) != output_set.end())
      output.push_back(nbits + i - 1);
    else if (variables[i] == "ALIVE")
      output.push_back(nbits + i - 1);
    else
      std::cout << "error: " << variables[i] << std::endl;
  }
  // print_int(input);
  // print_int(output);
}

void dfa::read_from_file(std::string filename) {
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
          nvars = stoi(fields[3]);
          // std::cout<<nvars<<std::endl;
        }
        if (strfind(line, "variables") && !strfind(line, "number")) {
          variables = split(line, " +");

        } else if (strfind(line, "states")) {
          fields = split(line, " +");
          nstates = stoi(fields[1]);
          // std::cout<<nstates<<std::endl;
        } else if (strfind(line, "initial")) {
          fields = split(line, " +");
          init = stoi(fields[1]);
          // std::cout<<init<<std::endl;
        } else if (strfind(line, "bdd nodes")) {
          fields = split(line, " ");
          nodes = stoi(fields[2]);
          // std::cout<<nodes<<std::endl;
        } else if (strfind(line, "final")) {
          fields = split(line, " +");
          int i = 1; // start at 1 to ignore "final" token
          while (i < fields.size()) {
            if (fields[i] == "1")
              finalstates.push_back(i - 1);
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
  // print_vec(smtbdd);
}

void dfa::construct_from_comp_front(std::string filename) {
  // Construct the BDD for the spec portion
  read_from_file(filename);
  nbits = state2bin(nstates - 1).length();
  construct_bdd_new();
}

void dfa::construct_from_comp_back(vbdd &S2S, vbdd &S2P, vbdd &Svars,
                                   vbdd &Ivars, vbdd &Ovars,
                                   std::vector<int> IS) {

  // substitute P from res, first create a substitution/projection vector, then
  // use the batch substitution function
  vbdd subnProj;
  // task dfa states
  for (int i = 0; i < nbits; i++) {
    subnProj.push_back(bddvars[i]);
  }
  // propositions (aka task variables)
  assert(S2P.size() == nvars);
  for (int i = 0; i < nvars; i++) {
    // TODO: We need to make sure the variables line up!!!!
    subnProj.push_back(S2P[i]);
  }
  // domain state variables
  for (auto &v : Svars) {
    subnProj.push_back(v);
  }
  // human action variables
  for (auto &v : Ivars) {
    subnProj.push_back(v);
  }
  // robot action variables
  for (auto &v : Ovars) {
    subnProj.push_back(v);
  }
  for (int i = 0; i < res.size(); i++) {
    res[i] = res[i].VectorCompose(subnProj);
  }

  // append the propositions to res
  res.insert(res.end(), S2P.begin(), S2P.end());
  // append S2S to res
  res.insert(res.end(), S2S.begin(), S2S.end());

  // fix the other variables (nvars, nbits, init, etc)
  // std::cout<<"constructing bdd with
  // "<<bddvars.size()<<"variables"<<std::endl;
  bddvars.insert(bddvars.end(), Svars.begin(), Svars.end());
  bddvars.insert(bddvars.end(), Ivars.begin(), Ivars.end());
  bddvars.insert(bddvars.end(), Ovars.begin(), Ovars.end());
  // std::cout<<"constructing bdd with
  // "<<bddvars.size()<<"variables"<<std::endl;
  // make init bitvector (final states is a bdd, does not need change)
  initbv = new int[nbits + nvars + Svars.size()];
  int temp = init;
  for (int i = nbits - 1; i >= 0; i--) {
    initbv[i] = temp % 2;
    temp = temp / 2;
  }
  for (int i = 0; i < nvars; i++) {
    initbv[i + nbits] = 0;
  }
  for (int i = 0; i < IS.size(); i++) {
    initbv[i + nbits + nvars] = IS[i];
  }
  nbits = nbits + nvars +
          Svars.size(); // TODO: can we eliminate the propositions completely?
  nvars = Ivars.size() + Ovars.size();

  // add indices for input and output
  input.clear();
  output.clear();
  for (int i = 0; i < Ivars.size(); i++) {
    input.push_back(i + nbits);
  }
  for (int i = 0; i < Ovars.size(); i++) {
    output.push_back(i + nbits + Ivars.size());
  }
}

void dfa::recur(int index, item tmp) {
  if (smtbdd[index][0] == -1) {
    while (tmp.size() < nbits + nvars)
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
  while (tmp.size() < (nbits + v))
    tmp.push_back(2); // 0:false 1:true 2:don't care
  tmp.push_back(0);
  recur(index, tmp);
}

void dfa::recur_right(int index, item tmp, int v) {
  while (tmp.size() < (nbits + v)) //
    tmp.push_back(2);              // 0:false 1:true 2:don't care
  tmp.push_back(1);
  recur(index, tmp);
}

void dfa::get_bdd() {
  for (int i = 0; i < nstates; i++) {
    int index = behaviour[i];
    item tmp;
    push_states(i, tmp);
    recur(index, tmp);
  }
}

void dfa::push_states(int i, item &tmp) {
  std::string s = state2bin(i);
  for (int j = 0; j < nbits - s.length(); j++)
    tmp.push_back(0);
  for (int j = 0; j < s.length(); j++) {
    int t = int(s[j]) - 48;
    tmp.push_back(t);
  }
}

void dfa::print(std::vector<std::string> &v) {
  for (size_t n = 0; n < v.size(); n++)
    std::cout << v[n] << " ";
  std::cout << std::endl;
}

void dfa::print_int(std::vector<int> &v) {
  for (size_t n = 0; n < v.size(); n++)
    std::cout << v[n] << " ";
  std::cout << std::endl;
}

void dfa::print_vec(std::vector<item> &v) {
  for (size_t n = 0; n < v.size(); n++)
    print_int(v[n]);
  std::cout << std::endl;
}

bool dfa::strfind(std::string str, std::string target) {
  size_t found = str.find(target);
  if (found != std::string::npos)
    return true;
  else
    return false;
}

std::string dfa::state2bin(int n) {
  std::string res;
  while (n) {
    res.push_back((n & 1) + '0');
    n >>= 1;
  }

  if (res.empty())
    res = "0";
  else
    std::reverse(res.begin(), res.end());
  // std::cout<<res<<std::endl;
  return res;
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

  for (int i = 0; i < nbits + nvars; i++) {
    CUDD::BDD b = mgr->bddVar();
    bddvars.push_back(b);
  }

  for (int i = 0; i < nbits; i++) {
    CUDD::BDD d = mgr->bddZero();
    res.push_back(d);
  }
  // std::cout<<"bddvars.length: "<<bddvars.size()<<std::endl;

  for (int i = 0; i < bdd.size(); i++) {
    for (int j = 0; j < nbits; j++) {
      if (bdd[i][nbits + nvars + j] == 1) {
        CUDD::BDD tmp = mgr->bddOne();
        for (int m = 0; m < nbits + nvars; m++) {
          if (bdd[i][m] != 2) {
            tmp *= var2bddvar(bdd[i][m], m);
          }
        }
        res[j] += tmp;
      }
    }
  }
}

void dfa::construct_bdd_new() {
  for (int i = 0; i < nbits + nvars; i++) {
    CUDD::BDD b = mgr->bddVar();
    bddvars.push_back(b);
    // dumpdot(b, to_string(i));
  }
  // std::cout<<"constructing bdd with
  // "<<bddvars.size()<<"variables"<<std::std::endl;

  for (int i = 0; i < nbits; i++) {
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

  for (int i = 0; i < nbits; i++) {
    for (int j = 0; j < nstates; j++) {
      CUDD::BDD tmp = mgr->bddOne();
      std::string bins = state2bin(j);
      int offset = nbits - bins.size();
      for (int m = 0; m < offset; m++) {
        tmp = tmp * var2bddvar(0, m);
      }
      for (int m = 0; m < bins.size(); m++) {
        tmp = tmp * var2bddvar(int(bins[m]) - 48, m + offset);
      }
      // dumpdot(tmp, "res-state "+to_string(behaviour[j])+to_string(i));
      // dumpdot(tBDD[behaviour[j]][i], "res-bdd
      // "+to_string(behaviour[j])+to_string(i));
      tmp = tmp * tBDD[behaviour[j]][i];
      res[i] = res[i] + tmp;
      // dumpdot(res[i], "res "+to_string(i));
    }
    // dumpdot(root, "res "+to_string(i));
  }

  finalstatesBDD = mgr->bddZero();
  for (int i = 0; i < finalstates.size(); i++) {
    CUDD::BDD ac = state2bdd(finalstates[i]);
    finalstatesBDD += ac;
  }
}

CUDD::BDD dfa::state2bdd(int s) {
  std::string bin = state2bin(s);
  CUDD::BDD b = mgr->bddOne();
  int nzero = nbits - bin.length();
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
    for (int m = 0; m < nbits - bins.size(); m++) {
      b.push_back(mgr->bddZero());
    }
    for (int i = 0; i < bins.size(); i++) {
      if (bins[i] == '0')
        b.push_back(mgr->bddZero());
      else if (bins[i] == '1')
        b.push_back(mgr->bddOne());
      else
        std::cout << "error binary state" << std::endl;
    }
    tBDD[index] = b;
    return b;
  } else {
    int rootindex = smtbdd[index][0];
    int leftindex = smtbdd[index][1];
    int rightindex = smtbdd[index][2];
    CUDD::BDD root = bddvars[rootindex + nbits];
    // dumpdot(root, "test");
    vbdd left = try_get(leftindex);
    // for(int l = 0; l < left.size(); l++)
    // dumpdot(left[l], "left"+to_string(l));
    vbdd right = try_get(rightindex);
    // for(int l = 0; l < left.size(); l++)
    // dumpdot(right[l], "right"+to_string(l));
    assert(left.size() == right.size());
    for (int i = 0; i < left.size(); i++) {
      CUDD::BDD tmp;
      tmp = root.Ite(right[i], left[i]); // Assume this is correct
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

} // namespace lydia
} // namespace whitemech
