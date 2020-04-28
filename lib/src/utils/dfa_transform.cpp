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

#include "utils/dfa_transform.hpp"
#include "cpphoafparser/consumer/hoa_consumer_print.hh"
#include <cstdio>
#include <graphviz/gvc.h>
#include <numeric>
#include <queue>
#include <utils/misc.hpp>
#include <utils/print.hpp>
#include <utils/strings.hpp>

namespace whitemech {
namespace lydia {

void interpretation_set_to_vect(const interpretation_set &src,
                                interpretation &dest) {
  std::fill(dest.begin(), dest.end(), 0);
  for (auto variable : src) {
    dest[variable] = 1;
  }
}

void dfa_to_graphviz(const dfa &automaton, const std::string &output_filename,
                     const std::string &format) {
  Agraph_t *g;
  GVC_t *gvc;
  Agnode_t *n, *m;
  Agedge_t *e;
  Agsym_t *a;
  FILE *file_ptr;
  file_ptr = fopen(output_filename.data(), "w");
  if (file_ptr == nullptr) {
    perror("Error opening file");
    return;
  }

  // generate all the symbols
  interpretation symbol(automaton.nb_variables);
  auto all_variables = std::set<int>();
  for (int i = 0; i < automaton.nb_variables; i++)
    all_variables.insert(i);
  auto all_symbols = powerset<int>(all_variables);

  g = agopen(strdup("automaton"), Agdirected, 0); // NOLINT

  // draw initial state
  n = agnode(g, strdup("fake"), 1); // NOLINT
  agsafeset(n, strdup("shape"), strdup("point"), strdup(""));
  auto initial_state_str = std::to_string(automaton.initial_state);
  m = agnode(g, initial_state_str.data(), 1);
  agedge(g, n, m, ("fake->" + initial_state_str).data(), 1);

  // do BFS over the automaton
  std::set<int> discovered;
  std::queue<int> to_be_visited;
  to_be_visited.push(automaton.initial_state);
  while (!to_be_visited.empty()) {
    auto state = to_be_visited.front();
    to_be_visited.pop();
    auto state_str = std::to_string(state);
    n = agnode(g, state_str.data(), 1);
    if (automaton.is_final(state)) {
      agsafeset(n, strdup("shape"), strdup("doublecircle"), strdup(""));
    }
    for (const interpretation_set &set_symbol : all_symbols) {
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

void dfa_to_hoa(const dfa &automaton, std::ostream &o) {
  auto printer = cpphoafparser::HOAConsumerPrint(o);
  printer.notifyHeaderStart("v1");
  printer.setNumberOfStates(automaton.nb_states);
  printer.addStartStates({static_cast<unsigned int>(automaton.initial_state)});
  printer.provideAcceptanceName("Rabin", {1});

  auto fin0 = cpphoafparser::HOAConsumer::acceptance_expr::Atom(
      cpphoafparser::AtomAcceptance::Fin(0));
  auto inf1 = cpphoafparser::HOAConsumer::acceptance_expr::Atom(
      cpphoafparser::AtomAcceptance::Inf(1));
  printer.setAcceptanceCondition(2, fin0 & inf1);

  std::vector<int> range(automaton.nb_variables);
  std::vector<std::string> ap_names;
  ap_names.reserve(automaton.nb_variables);
  std::iota(range.begin(), range.end(), 0);
  std::transform(std::begin(range), std::end(range),
                 std::back_inserter(ap_names),
                 [](int i) { return std::to_string(i); });
  printer.setAPs(ap_names);
  printer.notifyBodyStart();
  // TODO do body according to automaton
}

} // namespace lydia
} // namespace whitemech
