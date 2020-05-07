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

#include "dfa_transform/dfa_state.hpp"
#include "pl/logic.hpp"
#include <atom_visitor.hpp>
#include <dfa.hpp>
#include <dfa_transform/translate.hpp>
#include <nnf.hpp>
#include <queue>
#include <utility>
#include <utils/misc.hpp>

namespace whitemech {
namespace lydia {

std::shared_ptr<dfa> to_dfa(const LDLfFormula &formula, const CUDD::Cudd &mgr) {
  //  build initial state of the DFA.
  auto formula_nnf = to_nnf(formula);
  set_formulas initial_state_formulas{formula_nnf};
  dfa_state_ptr initial_state =
      std::make_shared<DFAState>(initial_state_formulas);

  // find all atoms
  set_atoms_ptr atoms = find_atoms(*formula_nnf);
  map_atoms_ptr atom2index;
  int index = 0;
  for (const auto &atom : atoms)
    atom2index[atom] = index++;
  auto all_interpretations = powerset<atom_ptr, SharedComparator>(atoms);

  // TODO max number of bits
  std::shared_ptr<dfa> automaton = std::make_shared<dfa>(mgr, 10, atoms.size());
  automaton->add_state();
  automaton->set_initial_state(1);

  //  Check if the initial state is final
  if (initial_state->is_final()) {
    automaton->set_final_state(1, true);
  }

  // BFS exploration of the automaton.
  map_dfa_states discovered;
  std::queue<std::pair<dfa_state_ptr, int>> to_be_visited;
  to_be_visited.push(std::make_pair(initial_state, 1));
  while (!to_be_visited.empty()) {
    auto pair = to_be_visited.front();
    to_be_visited.pop();
    const dfa_state_ptr current_state = pair.first;
    auto current_state_index = pair.second;
    /*
     * TODO: naive implementation: do a loop for every interpretation
     *       improvement: delta function returns a list of possible successors
     *                    (no interpretation to provide)
     */
    for (const auto &i : all_interpretations) {
      const dfa_state_ptr next_state = current_state->next_state(i);
      // update states/transitions
      int next_state_index = 0;
      if (discovered.find(next_state) == discovered.end()) {
        next_state_index = automaton->add_state();
        discovered[next_state] = next_state_index;
        to_be_visited.push(std::make_pair(next_state, next_state_index));
        if (next_state->is_final()) {
          automaton->set_final_state(next_state_index, true);
        }
      } else {
        next_state_index = discovered[next_state];
      }

      interpretation_set x{};
      for (const atom_ptr &atom : i)
        x.insert(atom2index[atom]);
      automaton->add_transition(current_state_index, x, next_state_index,
                                false);
    }
  }

  return automaton;
}

std::shared_ptr<dfa> to_dfa_sat(const LDLfFormula &formula,
                                const CUDD::Cudd &mgr) {
  //  build initial state of the DFA.
  auto formula_nnf = to_nnf(formula);
  set_formulas initial_state_formulas{formula_nnf};
  dfa_state_ptr initial_state =
      std::make_shared<DFAState>(initial_state_formulas);

  // find all atoms
  set_atoms_ptr atoms = find_atoms(*formula_nnf);
  map_atoms_ptr atom2index;
  int index = 0;
  for (const auto &atom : atoms)
    atom2index[atom] = index++;

  std::shared_ptr<dfa> automaton = std::make_shared<dfa>(mgr, 10, atoms.size());
  automaton->add_state();
  automaton->set_initial_state(1);

  //  Check if the initial state is final
  if (initial_state->is_final()) {
    automaton->set_final_state(1, true);
  }

  // BFS exploration of the automaton.
  map_dfa_states discovered;
  std::queue<std::pair<dfa_state_ptr, int>> to_be_visited;
  to_be_visited.push(std::make_pair(initial_state, 1));
  while (!to_be_visited.empty()) {
    auto pair = to_be_visited.front();
    to_be_visited.pop();
    const dfa_state_ptr current_state = pair.first;
    auto current_state_index = pair.second;
    vec_dfa_states next_states;
    std::vector<set_atoms_ptr> symbols;
    const auto &next_transitions = current_state->next_transitions();
    for (const auto &symbol_state : next_transitions) {
      const auto &symbol = symbol_state.first;
      const auto &next_state = symbol_state.second;
      // update states/transitions
      int next_state_index = 0;
      if (discovered.find(next_state) == discovered.end()) {
        next_state_index = automaton->add_state();
        discovered[next_state] = next_state_index;
        to_be_visited.push(std::make_pair(next_state, next_state_index));
        if (next_state->is_final()) {
          automaton->set_final_state(next_state_index, true);
        }
      } else {
        next_state_index = discovered[next_state];
      }

      interpretation_set x{};
      for (const atom_ptr &atom : symbol)
        x.insert(atom2index[atom]);
      automaton->add_transition(current_state_index, x, next_state_index, true);
    }
  }

  return automaton;
}

} // namespace lydia
} // namespace whitemech