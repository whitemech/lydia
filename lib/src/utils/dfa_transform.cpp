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

#include <lydia/utils/dfa_transform.hpp>

namespace whitemech::lydia {

void interpretation_set_to_vect(const interpretation_set& src,
                                interpretation& dest) {
  std::fill(dest.begin(), dest.end(), 0);
  for (auto variable : src) {
    dest[variable] = 1;
  }
}

void dfa_to_graphviz(const abstract_dfa& automaton,
                     const std::string& output_filename,
                     const std::string& format) {
  Agraph_t* g;
  GVC_t* gvc;
  Agnode_t *n, *m;
  Agedge_t* e;
  Agsym_t* a;
  FILE* file_ptr;
  file_ptr = fopen(output_filename.data(), "w");
  if (file_ptr == nullptr) {
    perror("Error opening file");
    return;
  }

  // generate all the symbols
  interpretation symbol(automaton.get_nb_variables());
  auto all_variables = std::set<int>();
  for (int i = 0; i < automaton.get_nb_variables(); i++)
    all_variables.insert(i);
  auto all_symbols = powerset<int>(all_variables);

  g = agopen(strdup("automaton"), Agdirected, 0); // NOLINT

  // draw initial state
  n = agnode(g, strdup("fake"), 1); // NOLINT
  agsafeset(n, strdup("shape"), strdup("point"), strdup(""));
  auto initial_state_str = std::to_string(automaton.get_initial_state());
  m = agnode(g, initial_state_str.data(), 1);
  agedge(g, n, m, ("fake->" + initial_state_str).data(), 1);

  // do BFS over the automaton
  std::set<int> discovered;
  std::queue<int> to_be_visited;
  to_be_visited.push(automaton.get_initial_state());
  discovered.insert(automaton.get_initial_state());
  while (!to_be_visited.empty()) {
    auto state = to_be_visited.front();
    to_be_visited.pop();
    auto state_str = std::to_string(state);
    n = agnode(g, state_str.data(), 1);
    if (automaton.is_final(state)) {
      agsafeset(n, strdup("shape"), strdup("doublecircle"), strdup(""));
    }
    for (const interpretation_set& set_symbol : all_symbols) {
      interpretation_set_to_vect(set_symbol, symbol);
      int successor = automaton.get_successor(state, symbol);
      if (discovered.find(successor) == discovered.end()) {
        to_be_visited.push(successor);
        discovered.insert(successor);
      }
      auto successor_str = std::to_string(successor);
      m = agnode(g, successor_str.data(), 1);
      if (automaton.is_final(successor)) {
        agsafeset(m, strdup("shape"), strdup("doublecircle"), strdup(""));
      }
      auto symbol_str = interpretation2string(symbol);
      e = agedge(g, n, m, nullptr, 1);
      agsafeset(e, strdup("label"), symbol_str.data(), strdup(""));
    }
  }

  gvc = gvContext();
  gvLayout(gvc, g, "dot");
  gvRender(gvc, g, format.data(), file_ptr);
  gvFreeLayout(gvc, g);
  agclose(g);
  gvFreeContext(gvc);
}

void dumpdot(const CUDD::Cudd& mgr, const CUDD::BDD& b,
             const std::vector<const char*>& inames,
             const std::string& filename) {
  FILE* fp = fopen(filename.c_str(), "w");
  std::vector<CUDD::BDD> single(1);
  single[0] = b;
  mgr.DumpDot(single, inames.data(), nullptr, fp);
  fclose(fp);
}

void bdd2dot(const dfa& automaton, const std::vector<std::string>& names,
             const std::string& directory) {
  const auto size = automaton.root_bdds.size();
  std::vector<const char*> inames;
  inames.reserve(names.size());
  std::transform(names.begin(), names.end(), std::back_inserter(inames),
                 [](const std::string& s) { return s.c_str(); });
  for (int i = 0; i < size; i++) {
    std::string filename = directory + "/" + std::to_string(i);
    dumpdot(automaton.mgr, automaton.root_bdds[i], inames, filename);
    std::system(("dot -Tsvg " + filename + " > " + filename + ".svg").c_str());
  }
}

void dfa_to_bdds(const dfa& automaton, const std::string& directory_path) {
  auto names = std::vector<std::string>();
  names.reserve(automaton.nb_bits + automaton.get_nb_variables());
  // populate bit names
  for (int i = 0; i < automaton.nb_bits; i++) {
    names.push_back("b" + std::to_string(i));
  }
  // populate variable names
  names.insert(names.end(), automaton.variables.begin(),
               automaton.variables.end());
  bdd2dot(automaton, names, directory_path);
}

} // namespace whitemech::lydia
