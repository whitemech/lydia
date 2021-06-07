#pragma once
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

#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/strategies/bdd/delta_bdd.hpp>

namespace whitemech::lydia {

class BDDStrategy : public Strategy {
private:
  AstManager* current_context_;

public:
  const CUDD::Cudd& mgr;
  const size_t max_nb_bits;
  dfa_ptr automaton;
  std::vector<atom_ptr> id2atoms;
  std::map<atom_ptr, size_t, SharedComparator> atom2ids;

  vec_bdd subformula_bddvars;
  std::vector<ldlf_ptr> id2subformula;
  std::map<ldlf_ptr, size_t, SharedComparator> subformula2id;

  explicit BDDStrategy(const CUDD::Cudd& mgr, uint32_t max_nb_bits = 10)
      : mgr{mgr}, max_nb_bits{max_nb_bits} {};

  std::shared_ptr<abstract_dfa> to_dfa(const LDLfFormula& formula) override;

  std::map<nfa_state_ptr, CUDD::BDD, SharedComparator>
  next_transitions(const NFAState& state);
  std::vector<std::pair<dfa_state_ptr, CUDD::BDD>>
  next_transitions(const DFAState& state);

  void add_transition(int from_index, CUDD::BDD guard, int to_index);
};

} // namespace whitemech::lydia