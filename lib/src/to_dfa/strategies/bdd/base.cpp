/*
 * This file is part of Lydia.
 *
 * Lydia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Lydia is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lydia.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <lydia/dfa/dfa.hpp>
#include <lydia/logic/atom_visitor.hpp>
#include <lydia/logic/nnf.hpp>
#include <lydia/to_dfa/delta_symbolic.hpp>
#include <lydia/to_dfa/dfa_state.hpp>
#include <lydia/to_dfa/strategies/bdd/base.hpp>
#include <lydia/utils/strings.hpp>
#include <memory>
#include <queue>

namespace whitemech::lydia {

std::shared_ptr<abstract_dfa> BDDStrategy::to_dfa(const LDLfFormula& formula) {
  auto formula_nnf = to_nnf(formula);
  current_context_ = &formula.ctx();
  set_formulas initial_state_formulas{formula_nnf};
  dfa_state_ptr initial_state =
      std::make_shared<DFAState>(*current_context_, initial_state_formulas);

  // find all atoms
  set_atoms_ptr atoms = find_atoms(*formula_nnf);
  int index = 0;
  for (const auto& atom : atoms) {
    atom2ids[atom] = index;
    id2atoms.push_back(atom);
    index++;
  }

  automaton = std::make_shared<dfa>(mgr, max_nb_bits, atoms.size());
  automaton->add_state();
  automaton->set_initial_state(1);

  //  Check if the initial state is final
  if (initial_state->is_final()) {
    automaton->set_final_state(1, true);
  }

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

    const auto& next_transitions = this->next_transitions(*current_state);
    for (const auto& symbol_state : next_transitions) {
      const auto& next_state = symbol_state.first;
      const auto& symbol = symbol_state.second;
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

      add_transition(current_state_index, symbol, next_state_index);
    }
  }
  return automaton;
}

std::vector<std::pair<dfa_state_ptr, CUDD::BDD>>
BDDStrategy::next_transitions(const DFAState& state) {
  std::vector<std::pair<dfa_state_ptr, CUDD::BDD>> result;
  std::map<set_atoms_ptr, set_nfa_states, cmp_set_of_ptr> symbol2nfastates;
  set_dfa_states discovered;
  set_nfa_states nfa_states;
  set_atoms_ptr symbol;
  std::map<nfa_state_ptr, CUDD::BDD, SharedComparator> all_transitions;
  for (const auto& nfa_state : state.states) {
    const auto& next_transitions = this->next_transitions(*nfa_state);
    for (const auto& pair : next_transitions) {
      if (all_transitions.find(pair.first) == all_transitions.end()) {
        all_transitions[pair.first] = automaton->mgr.bddZero();
      }
      all_transitions[pair.first] += pair.second;
    }
  }

  std::vector<std::pair<nfa_state_ptr, CUDD::BDD>> all_transitions_vec(
      all_transitions.begin(), all_transitions.end());
  size_t N = all_transitions_vec.size();
  if (N == 0)
    return result;
  size_t nb_combinations = pow(2, N);
  for (size_t i = 0; i < nb_combinations; i++) {
    const auto& combination = state2bin(i, N);
    set_nfa_states current_state{};
    CUDD::BDD current_label = automaton->mgr.bddOne();
    for (size_t j = 0; j < combination.size(); j++) {
      bool membership_bit = combination[j] == '1';
      const auto& state_label = all_transitions_vec[j];
      current_label *=
          (membership_bit ? state_label.second : !state_label.second);
      if (membership_bit)
        current_state.insert(state_label.first);
    }
    if (current_label.IsZero())
      continue;
    dfa_state_ptr current_dfa_state =
        std::make_shared<DFAState>(state.context, current_state);
    result.emplace_back(current_dfa_state, current_label);
  }
  return result;
}

std::map<nfa_state_ptr, CUDD::BDD, SharedComparator>
BDDStrategy::next_transitions(const NFAState& state) {
  std::map<nfa_state_ptr, CUDD::BDD, SharedComparator> result;
  set_prop_formulas setPropFormulas;
  for (const auto& f : state.formulas) {
    const auto& delta_formula = delta_symbolic(*f, false);
    setPropFormulas.insert(delta_formula);
  }
  auto and_ = current_context_->makePropAnd(setPropFormulas);
  CUDD::BDD successor_fun = bdd_delta_symbolic(*this, *and_);
  if (successor_fun.IsZero())
    return result;

  // compute prime implicants
  int* cube = nullptr;
  DdGen* g = Cudd_FirstPrime(mgr.getManager(), successor_fun.getNode(),
                             successor_fun.getNode(), &cube);
  size_t nb_all_variables =
      automaton->nb_bits + automaton->get_nb_variables() + id2subformula.size();
  size_t nb_automaton_variables =
      automaton->nb_bits + automaton->get_nb_variables();
  if (g != nullptr) {
    do {
      CUDD::BDD symbol = mgr.bddOne();
      set_formulas next_state;
      for (size_t i = automaton->nb_bits; i < nb_automaton_variables; i++) {
        if (cube[i] == 2)
          continue;
        CUDD::BDD tmp = automaton->bddvars[i];
        if (cube[i] == 0) {
          tmp = !tmp;
        }
        symbol &= tmp;
      }

      for (size_t i = nb_automaton_variables; i < nb_all_variables; i++) {
        if (cube[i] == 1) {
          next_state.insert(id2subformula[i - nb_automaton_variables]);
        }
      }

      auto nfa_state = std::make_shared<NFAState>(state.context, next_state);
      if (result.find(nfa_state) == result.end()) {
        result[nfa_state] = mgr.bddZero();
      }
      result[nfa_state] += symbol;
    } while (Cudd_NextPrime(g, &cube));
  }

  return result;
}

void BDDStrategy::add_transition(int from_index, CUDD::BDD guard,
                                 int to_index) {
  std::string to_binary = state2bin(to_index, automaton->nb_bits, true);
  //  Update each root BDD.
  guard = guard * automaton->state2bdd(from_index);
  assert(automaton->nb_bits == to_binary.length());
  for (int i = 0; i < automaton->nb_bits; i++) {
    bool result = int(to_binary[i]) - '0';
    if (result)
      automaton->root_bdds[i] += guard;
  }
}

} // namespace whitemech::lydia