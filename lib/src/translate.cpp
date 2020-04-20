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
#include <queue>
#include <translate.hpp>
#include <utility>
#include <utils/misc.hpp>

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
  set_atoms_ptr atoms = find_atoms(*formula_nnf);
  auto all_interpretations = powerset<atom_ptr, SharedComparator>(atoms);

  //  Check if the initial state is final
  if (initial_state->is_final()) {
    final_states.insert(initial_state);
  }

  // BFS exploration of the automaton.
  set_dfa_states visited;
  std::queue<dfa_state_ptr> to_be_visited;
  to_be_visited.push(initial_state);
  while (!to_be_visited.empty()) {
    const auto &current_state = to_be_visited.front();
    to_be_visited.pop();
    visited.insert(current_state);

    /*
     * TODO: naive implementation: do a loop for every interpretation
     *       improvement: delta function returns a list of possible successors
     *                    (no interpretation to provide)
     */
    for (const auto &i : all_interpretations) {
      const auto next_state = current_state->next_state(i);
      // update states/transitions
      states.insert(next_state);
      transitions.insert(std::tie(*current_state, i, *next_state));
      if (visited.find(next_state) == visited.end())
        to_be_visited.push(next_state);
    }
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
  assert(is_a<NFAState>(rhs));
  return unified_compare(this->formulas,
                         dynamic_cast<const NFAState &>(rhs).formulas);
}

hash_t NFAState::compute_hash_() const {
  hash_t seed = type_code_id;
  for (const auto &a : formulas)
    hash_combine<Basic>(seed, *a);
  return seed;
}

bool NFAState::is_final() const {
  // This will be put in conjunction with other formulas
  vec_prop_formulas args{std::make_shared<PropositionalTrue>(),
                         std::make_shared<PropositionalTrue>()};
  for (const auto &formula : formulas) {
    args.push_back(delta(*formula));
  }
  auto conjunction =
      PropositionalAnd(set_prop_formulas(args.begin(), args.end()));

  // to evaluate this formula, we just use an empty prop. interpretation.
  // this is because the delta with epsilon = true returns either true or false.
  auto empty = set_atoms_ptr();
  return eval(conjunction, empty);
}

set_nfa_states NFAState::next_states(const set_atoms_ptr &i) const {
  // This will be put in conjunction with other formulas
  vec_prop_formulas args{std::make_shared<PropositionalTrue>(),
                         std::make_shared<PropositionalTrue>()};
  for (const auto &formula : formulas) {
    args.push_back(delta(*formula));
  }
  auto conjunction =
      PropositionalAnd(set_prop_formulas(args.begin(), args.end()));

  set_nfa_states result;
  auto models = minimal_models(conjunction);
  for (const auto &model : models) {
    set_formulas tmp;
    for (const auto &atom : model) {
      assert(is_a<QuotedFormula>(*atom->symbol));
      tmp.insert(dynamic_cast<const QuotedFormula &>(*atom->symbol).formula);
    }
    result.emplace(std::make_shared<NFAState>(tmp));
  }

  return result;
}

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

} // namespace lydia
} // namespace whitemech