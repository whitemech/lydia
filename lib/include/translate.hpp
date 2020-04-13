#pragma once
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

#include "dfa.hpp"
#include "logic.hpp"

namespace whitemech {
namespace lydia {

dfa *to_dfa(LDLfFormula &formula);

/*!
 * This class represents an NFA state in
 * the LDLf2NFA algorithm presented in [1].
 * This will be used in tandem with the DFAState
 * of the algorithm LDLf2DFA [2].
 *
 * [1] Brafman, Ronen I., Giuseppe De Giacomo, and Fabio Patrizi.
 *     "LTLf/LDLf non-markovian rewards."
 *     Thirty-Second AAAI Conference on Artificial Intelligence. 2018.
 * [2] Favorito, Marco. "Reinforcement learning for LTLf/LDLf goals:
 *     Theory and implementation." Master's thesis. DIAG, Sapienza Univ. Rome
 (2018).

 */
class NFAState {
public:
  set_formulas &formulas;
  explicit NFAState(set_formulas &formulas) : formulas{formulas} {};
};

/*!
 * This class represents a DFA state in
 * the on-the-fly LDLf2NFA algorithm presented in [1].
 * And in the algorithm LDLf2DFA [2].
 *
 * [1] Brafman, Ronen I., Giuseppe De Giacomo, and Fabio Patrizi.
 *     "LTLf/LDLf non-markovian rewards."
 *     Thirty-Second AAAI Conference on Artificial Intelligence. 2018.
 * [2] Favorito, Marco. "Reinforcement learning for LTLf/LDLf goals:
 *     Theory and implementation." Master's thesis. DIAG, Sapienza Univ. Rome
 * (2018).
 */
class DFAState {
public:
  DFAState();
};

} // namespace lydia
} // namespace whitemech
