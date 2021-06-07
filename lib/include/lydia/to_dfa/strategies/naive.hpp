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

#include <lydia/dfa/dfa.hpp>
#include <lydia/logic/atom_visitor.hpp>
#include <lydia/logic/nnf.hpp>
#include <lydia/logic/pl/base.hpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/dfa_state.hpp>
#include <lydia/utils/misc.hpp>
#include <queue>
#include <utility>

namespace whitemech::lydia {

class NaiveStrategy : public Strategy {
private:
  AstManager* current_context;
  const CUDD::Cudd& mgr;
  const size_t max_nb_bits;

public:
  explicit NaiveStrategy(const CUDD::Cudd& mgr, uint32_t max_nb_bits = 10)
      : mgr{mgr}, max_nb_bits{max_nb_bits} {};

  std::shared_ptr<abstract_dfa> to_dfa(const LDLfFormula& formula) override;

  /*!
   * Compute the next state, given a propositional interpretation.
   *
   * That is, compute the successor of each NFA state in the current DFA state.
   *
   * @param state the current DFA state
   * @param i the propositional interpretation (a set of propositional atoms).
   * @return the next DFA state.
   */
  static dfa_state_ptr next_state(const DFAState& state,
                                  const set_atoms_ptr& i);

  /*!
   * Compute the next states, given a propositional interpretation.
   *
   * That is, compute the successors of each NFA state in the current DFA state.
   *
   * @param state the current NFA state
   * @param i the propositional interpretation (a set of propositional atoms).
   * @return the next NFA states.
   */
  static set_nfa_states next_states(const NFAState& state,
                                    const set_atoms_ptr& i);
};

} // namespace whitemech::lydia