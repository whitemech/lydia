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

#include <delta.hpp>
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

  //  Check if the initial state is final

  return nullptr;
}

bool NFAState::is_equal(const Basic &rhs) const {
  return is_a<NFAState>(rhs) and
         unified_eq(this->formulas,
                    dynamic_cast<const NFAState &>(rhs).formulas);
}

int NFAState::compare(const Basic &rhs) const {
  return is_a<NFAState>(rhs) and
         unified_compare(this->formulas,
                         dynamic_cast<const NFAState &>(rhs).formulas);
}

hash_t NFAState::__hash__() const {
  hash_t seed = type_code_id;
  for (const auto &a : formulas)
    hash_combine<Basic>(seed, *a);
  return seed;
}

bool NFAState::is_final() const {
  // This will be put in conjunction with other formulas
  auto delta_epsilon_result = PropositionalTrue();
  for (const auto &formula : formulas) {
    auto delta_epsilon_result = delta(*formula, true);
    /*TODO instead of equality, we could implement
     * a function like "logical_equivalence".
     */
  }
  //  return delta_epsilon_result->is_equal(PropositionaTrue());
  return true;
}

hash_t DFAState::__hash__() const {
  hash_t seed = type_code_id;
  for (const auto &a : states)
    hash_combine<Basic>(seed, *a);
  return seed;
}

int DFAState::compare(const Basic &rhs) const {
  return is_a<DFAState>(rhs) and
         unified_compare(this->states,
                         dynamic_cast<const DFAState &>(rhs).states);
}

bool DFAState::is_equal(const Basic &rhs) const {
  return is_a<DFAState>(rhs) and
         unified_eq(this->states, dynamic_cast<const DFAState &>(rhs).states);
}

bool DFAState::is_final() const { return false; };

bool DFATransition::is_equal(const Basic &rhs) const {
  return is_a<DFATransition>(rhs) and
         unified_eq(this->transition,
                    dynamic_cast<const DFATransition &>(rhs).transition);
}

int DFATransition::compare(const Basic &rhs) const {
  return is_a<DFATransition>(rhs) and
         unified_compare(this->transition,
                         dynamic_cast<const DFATransition &>(rhs).transition);
};
} // namespace lydia
} // namespace whitemech