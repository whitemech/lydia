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

#include <lydia/to_dfa/dfa_state.hpp>

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

} // namespace lydia
} // namespace whitemech