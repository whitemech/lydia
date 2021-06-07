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

#include <lydia/logic/pl/models/base.hpp>
#include <lydia/logic/pl/models/naive.hpp>
#include <lydia/to_dfa/strategies/naive.hpp>

namespace whitemech::lydia {

std::shared_ptr<abstract_dfa>
NaiveStrategy::to_dfa(const LDLfFormula& formula) {
  //  build initial state of the DFA.
  auto formula_nnf = to_nnf(formula);
  current_context = &formula_nnf->ctx();
  set_formulas initial_state_formulas{formula_nnf};
  dfa_state_ptr initial_state =
      std::make_shared<DFAState>(*current_context, initial_state_formulas);

  // find all atoms
  set_atoms_ptr atoms = find_atoms(*formula_nnf);
  map_atoms_ptr atom2index;
  int index = 0;
  for (const auto& atom : atoms)
    atom2index[atom] = index++;
  auto all_interpretations = powerset<atom_ptr, SharedComparator>(atoms);

  // TODO max number of bits
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
    /*
     * TODO: naive implementation: do a loop for every interpretation
     *       improvement: delta function returns a list of possible successors
     *                    (no interpretation to provide)
     */
    for (const auto& i : all_interpretations) {
      const dfa_state_ptr next_state =
          NaiveStrategy::next_state(*current_state, i);
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
      for (const atom_ptr& atom : i)
        x.insert(atom2index[atom]);
      automaton->add_transition(current_state_index, x, next_state_index,
                                false);
    }
  }

  return automaton;
}

dfa_state_ptr NaiveStrategy::next_state(const DFAState& state,
                                        const set_atoms_ptr& i) {
  set_nfa_states successor_nfa_states;
  for (const auto& nfa_state : state.states) {
    auto successors = NaiveStrategy::next_states(*nfa_state, i);
    successor_nfa_states.insert(successors.begin(), successors.end());
  }
  return std::make_shared<DFAState>(state.context, successor_nfa_states);
}

set_nfa_states NaiveStrategy::next_states(const NFAState& state,
                                          const set_atoms_ptr& i) {
  // This will be put in conjunction with o ther formulas
  vec_prop_formulas args{state.context.makeTrue(), state.context.makeTrue()};
  for (const auto& formula : state.formulas) {
    args.push_back(delta(*formula, i));
  }
  auto conjunction =
      state.context.makePropAnd(set_prop_formulas(args.begin(), args.end()));

  set_nfa_states result;
  auto models = all_models<NaiveModelEnumerationStategy>(*conjunction);
  for (const auto& model : models) {
    set_formulas tmp;
    for (const auto& atom : model) {
      assert(is_a<QuotedFormula>(*atom->symbol));
      basic_ptr tmp_ptr =
          dynamic_cast<const QuotedFormula&>(*atom->symbol).formula;
      tmp.insert(std::static_pointer_cast<const LDLfFormula>(tmp_ptr));
    }
    result.emplace(std::make_shared<NFAState>(state.context, tmp));
  }

  return result;
}

} // namespace whitemech::lydia