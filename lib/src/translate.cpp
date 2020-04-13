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

#include <dfa.hpp>
#include <nnf.hpp>
#include <translate.hpp>

namespace whitemech {
namespace lydia {

dfa *to_dfa(LDLfFormula &formula) {
  auto formula_nnf = to_nnf(formula);
  set_formulas initial_state_formulas{formula_nnf};
  set_nfa_states nfa_states{
      std::make_shared<NFAState>(set_formulas{formula_nnf})};

  //  initialize data structure of the final DFA
  std::shared_ptr<DFAState> initial_state =
      std::make_shared<DFAState>(nfa_states);
  set_dfa_states final_states;
  set_dfa_states states{{initial_state}};
  set_dfa_transitions transitions;

  return nullptr;
}

} // namespace lydia
} // namespace whitemech