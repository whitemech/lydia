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
#include <lydia/pl/cnf.hpp>
#include <lydia/pl/models.hpp>
#include <lydia/to_dfa/delta_symbolic.hpp>
#include <lydia/to_dfa/strategies/sat.hpp>

namespace whitemech {
namespace lydia {

std::shared_ptr<dfa> SATStrategy::to_dfa(const LDLfFormula &formula) {
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

  std::shared_ptr<dfa> automaton =
      std::make_shared<dfa>(mgr, max_nb_bits, atoms.size());
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
    const auto &next_transitions = this->next_transitions(*current_state);
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

std::vector<std::pair<set_atoms_ptr, dfa_state_ptr>>
SATStrategy::next_transitions(const DFAState &state) {
  std::vector<std::pair<set_atoms_ptr, dfa_state_ptr>> result;
  std::map<set_atoms_ptr, set_nfa_states> symbol2nfastates;
  set_dfa_states discovered;
  set_nfa_states nfa_states;
  set_atoms_ptr symbol;
  for (const auto &nfa_state : state.states) {
    const auto &next_transitions = this->next_transitions(*nfa_state);
    for (const auto &symbol_states : next_transitions) {
      symbol = symbol_states.first;
      nfa_states = symbol_states.second;
      if (symbol2nfastates.find(symbol) == symbol2nfastates.end()) {
        symbol2nfastates[symbol] = set_nfa_states{};
      }
      symbol2nfastates[symbol].insert(nfa_states.begin(), nfa_states.end());
    }
  }

  for (const auto &pair : symbol2nfastates) {
    result.emplace_back(pair.first, std::make_shared<DFAState>(pair.second));
  }
  return result;
}

std::vector<std::pair<set_atoms_ptr, set_nfa_states>>
SATStrategy::next_transitions(const NFAState &state) {
  std::vector<std::pair<set_atoms_ptr, set_nfa_states>> result;
  std::map<set_atoms_ptr, set_nfa_states> symbol2nfastates;
  set_prop_formulas setPropFormulas;
  set_nfa_states v;
  for (const auto &f : state.formulas) {
    const auto &delta_formula = delta_symbolic(*f, false);
    setPropFormulas.insert(delta_formula);
  }
  auto and_ = to_cnf(*logical_and(setPropFormulas));
  const auto &all_minimal_models = all_models(*and_);
  for (const auto &model : all_minimal_models) {
    set_nfa_states nfa_states;
    set_formulas quoted_formulas;
    set_atoms_ptr symbol;
    for (const atom_ptr &ptr : model) {
      if (is_a<QuotedFormula>(*ptr->symbol))
        quoted_formulas.insert(
            dynamic_cast<const QuotedFormula &>(*ptr->symbol).formula);
      else
        symbol.insert(ptr);
    }

    auto successor = symbol2nfastates.find(symbol);
    if (successor == symbol2nfastates.end()) {
      symbol2nfastates[symbol] = set_nfa_states();
    }
    symbol2nfastates[symbol].insert(
        std::make_shared<NFAState>(quoted_formulas));
  }

  result.reserve(symbol2nfastates.size());
  for (const auto &pair : symbol2nfastates) {
    result.emplace_back(pair);
  }
  return result;
}

} // namespace lydia
} // namespace whitemech