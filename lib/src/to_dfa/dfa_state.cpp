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

namespace whitemech {
namespace lydia {

DFAState::DFAState(set_nfa_states states) : states{std::move(states)} {
  this->type_code_ = type_code_id;
}
DFAState::DFAState(const set_formulas &formulas)
    : states{set_nfa_states{std::make_shared<NFAState>(formulas)}} {
  this->type_code_ = type_code_id;
}

hash_t DFAState::compute_hash_() const {
  hash_t seed = type_code_id;
  for (const auto &a : states)
    hash_combine<Basic>(seed, *a);
  return seed;
}

int DFAState::compare(const Basic &rhs) const {
  assert(is_a<DFAState>(rhs));
  return unified_compare(this->states,
                         dynamic_cast<const DFAState &>(rhs).states);
}

bool DFAState::is_equal(const Basic &rhs) const {
  return is_a<DFAState>(rhs) and
         unified_eq(this->states, dynamic_cast<const DFAState &>(rhs).states);
}

bool DFAState::is_final() const {
  for (const auto &nfa_state : states) {
    if (nfa_state->is_final())
      return true;
  }
  return false;
}

dfa_state_ptr DFAState::next_state(const set_atoms_ptr &i) const {
  set_nfa_states successor_nfa_states;
  for (const auto &nfa_state : states) {
    auto successors = nfa_state->next_states(i);
    successor_nfa_states.insert(successors.begin(), successors.end());
  }
  return std::make_shared<DFAState>(successor_nfa_states);
}

std::vector<std::pair<set_atoms_ptr, dfa_state_ptr>>
DFAState::next_transitions() const {
  std::vector<std::pair<set_atoms_ptr, dfa_state_ptr>> result;
  std::map<set_atoms_ptr, set_nfa_states> symbol2nfastates;
  set_dfa_states discovered;
  set_nfa_states nfa_states;
  set_atoms_ptr symbol;
  for (const auto &nfa_state : this->states) {
    const auto &next_transitions = nfa_state->next_transitions();
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

} // namespace lydia
} // namespace whitemech