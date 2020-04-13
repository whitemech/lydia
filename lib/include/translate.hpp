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
#include "types.hpp"
#include <memory>
#include <utility>
#include <utils/compare.hpp>

namespace whitemech {
namespace lydia {

/*!
 *
 * Translate an LDLf formula into an DFA.
 *
 * This is one of the main procedures of the library.
 *
 * @param formula the LDLf formula.
 * @return the equivalent DFA.
 */
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
class NFAState : public Basic {
public:
  const static TypeID type_code_id = TypeID::t_NFAState;
  const set_formulas &formulas;
  explicit NFAState(const set_formulas &formulas) : formulas{formulas} {};

  void accept(Visitor &v) const override{};
  hash_t __hash__() const override {
    hash_t seed = type_code_id;
    for (const auto &a : formulas)
      hash_combine<Basic>(seed, *a);
    return seed;
  }

  int compare(const Basic &rhs) const override {
    return is_a<NFAState>(rhs) and
           unified_compare(this->formulas,
                           dynamic_cast<const NFAState &>(rhs).formulas);
  };
  bool is_equal(const Basic &rhs) const override {
    return is_a<NFAState>(rhs) and
           unified_eq(this->formulas,
                      dynamic_cast<const NFAState &>(rhs).formulas);
  };
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
class DFAState : public Basic {
public:
  const static TypeID type_code_id = TypeID::t_DFAState;
  const set_nfa_states &states;
  explicit DFAState(const set_nfa_states &states) : states{states} {};

  void accept(Visitor &v) const override{};
  hash_t __hash__() const override {
    hash_t seed = type_code_id;
    for (const auto &a : states)
      hash_combine<Basic>(seed, *a);
    return seed;
  }

  int compare(const Basic &rhs) const override {
    return is_a<DFAState>(rhs) and
           unified_compare(this->states,
                           dynamic_cast<const DFAState &>(rhs).states);
  };
  bool is_equal(const Basic &rhs) const override {
    return is_a<DFAState>(rhs) and
           unified_eq(this->states, dynamic_cast<const DFAState &>(rhs).states);
  };
};

class DFATransition : public Basic {
public:
  const static TypeID type_code_id = TypeID::t_DFATransition;
  tuple_dfa_transition transition;
  DFATransition(const DFAState &a, std::set<std::string> s, const DFAState &b)
      : transition{std::tie(a, s, b)} {};

  void accept(Visitor &v) const override{};
  hash_t __hash__() const override {
    // TODO
    return 0;
  }

  int compare(const Basic &rhs) const override {
    return is_a<DFATransition>(rhs) and
           unified_compare(this->transition,
                           dynamic_cast<const DFATransition &>(rhs).transition);
  };
  bool is_equal(const Basic &rhs) const override {
    return is_a<DFATransition>(rhs) and
           unified_eq(this->transition,
                      dynamic_cast<const DFATransition &>(rhs).transition);
  };
};

} // namespace lydia
} // namespace whitemech
