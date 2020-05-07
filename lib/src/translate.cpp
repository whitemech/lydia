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

#include "pl/eval.hpp"
#include "pl/logic.hpp"
#include "pl/models.hpp"
#include "pl/models_sat.hpp"
#include <atom_visitor.hpp>
#include <delta.hpp>
#include <delta_sat.hpp>
#include <dfa.hpp>
#include <nnf.hpp>
#include <pl/cnf.hpp>
#include <queue>
#include <translate.hpp>
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
    args.push_back(delta(*formula, i));
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

std::vector<std::pair<set_atoms_ptr, set_nfa_states>>
NFAState::next_transitions() const {
  std::vector<std::pair<set_atoms_ptr, set_nfa_states>> result;
  std::map<set_atoms_ptr, set_nfa_states> symbol2nfastates;
  set_prop_formulas setPropFormulas;
  set_nfa_states v;
  for (const auto &f : formulas) {
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