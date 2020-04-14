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

#include <atom_visitor.hpp>
#include <delta.hpp>
#include <dfa.hpp>
#include <nnf.hpp>
#include <translate.hpp>
#include <utility>

namespace whitemech {
namespace lydia {

dfa *to_dfa(LDLfFormula &formula) {
  //  build initial state of the DFA.
  auto formula_nnf = to_nnf(formula);
  set_formulas initial_state_formulas{formula_nnf};
  dfa_state_ptr initial_state =
      std::make_shared<DFAState>(initial_state_formulas);

  //  initialize data structure of the final DFA
  set_dfa_states final_states;
  set_dfa_states states{{initial_state}};
  set_dfa_transitions transitions;

  // find all atoms
  set_atoms atoms = find_atoms(*formula_nnf);

  //  Check if the initial state is final
  if (initial_state->is_final()) {
    final_states.insert(initial_state);
  }

  // find all the atoms of the formula.

  // while the set of states or the transition function do not change
  size_t nb_states = 0;
  size_t nb_transitions = 0;
  while (nb_states != states.size() && nb_transitions != transitions.size()) {
    nb_states = states.size();
    nb_transitions = transitions.size();
  }
  return nullptr;
}

NFAState::NFAState(set_formulas formulas) : formulas{std::move(formulas)} {
  this->type_code_ = type_code_id;
};

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
  vec_prop_formulas args{std::make_shared<PropositionalTrue>()};
  for (const auto &formula : formulas) {
    args.push_back(delta(*formula, true));
  }
  auto conjunction =
      PropositionalAnd(set_prop_formulas(args.begin(), args.end()));
  /*TODO the following line WON'T WORK.
   * Instead of equality, we could implement
   * a function like "logical_equivalence".
   */
  return conjunction.is_equal(PropositionalTrue());
}

DFAState::DFAState(set_nfa_states states) : states{std::move(states)} {}
DFAState::DFAState(const set_formulas &formulas)
    : states{set_nfa_states{std::make_shared<NFAState>(formulas)}} {
  this->type_code_ = type_code_id;
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

bool DFAState::is_final() const {
  for (const auto &nfa_state : states) {
    if (nfa_state->is_final())
      return true;
  }
  return false;
}

bool DFATransition::is_equal(const Basic &rhs) const {
  return is_a<DFATransition>(rhs) and
         unified_eq(this->transition,
                    dynamic_cast<const DFATransition &>(rhs).transition);
}

int DFATransition::compare(const Basic &rhs) const {
  return is_a<DFATransition>(rhs) and
         unified_compare(this->transition,
                         dynamic_cast<const DFATransition &>(rhs).transition);
}

DFATransition::DFATransition(const DFAState &a, std::set<std::string> s,
                             const DFAState &b)
    : transition{std::tie(a, s, b)} {
  this->type_code_ = type_code_id;
}

} // namespace lydia
} // namespace whitemech