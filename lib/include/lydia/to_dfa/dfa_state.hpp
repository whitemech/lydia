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

#include <lydia/ldlf/logic.hpp>
#include <lydia/to_dfa/nfa_state.hpp>
#include <lydia/types.hpp>
#include <lydia/utils/compare.hpp>
#include <memory>
#include <utility>

namespace whitemech {
namespace lydia {

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
class DFAState : public Basic {
public:
  const static TypeID type_code_id = TypeID::t_DFAState;
  const set_nfa_states states;
  explicit DFAState(set_nfa_states states);
  explicit DFAState(const set_formulas &formulas);

  void accept(Visitor &v) const override{};
  hash_t compute_hash_() const override;
  int compare(const Basic &rhs) const override;
  bool is_equal(const Basic &rhs) const override;

  /*!
   * Check if the state is final
   *
   * That means calling the delta function with epsilon.
   * As stated in (Brafman et al. 2018), in the on-the-fly
   * construction this is equivalent to check if the NFA state
   * {true} is in the state (after applying the delta function).
   *
   * @return whether the state is final or not.
   */
  bool is_final() const;
};

} // namespace lydia
} // namespace whitemech