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

#include <lydia/logic/pl/cnf.hpp>
#include <lydia/logic/pl/models/base.hpp>
#include <lydia/to_dfa/delta_symbolic.hpp>
#include <lydia/to_dfa/nfa_state.hpp>

namespace whitemech::lydia {

bool NFAState::is_equal(const Basic& rhs) const {
  return is_a<NFAState>(rhs) and
         unified_eq(this->formulas,
                    dynamic_cast<const NFAState&>(rhs).formulas);
}

int NFAState::compare_(const Basic& rhs) const {
  assert(is_a<NFAState>(rhs));
  return unified_compare(this->formulas,
                         dynamic_cast<const NFAState&>(rhs).formulas);
}

hash_t NFAState::compute_hash_() const {
  hash_t seed = type_code_id;
  for (const auto& a : formulas)
    hash_combine<Basic>(seed, *a);
  return seed;
}

bool NFAState::is_final() const {
  // This will be put in conjunction with other formulas
  vec_prop_formulas args{context.makeTrue(), context.makeTrue()};
  for (const auto& formula : formulas) {
    args.push_back(delta(*formula));
  }
  auto conjunction =
      context.makePropAnd(set_prop_formulas(args.begin(), args.end()));

  // to evaluate this formula, we just use an empty prop. interpretation.
  // this is because the delta with epsilon = true returns either true or false.
  auto empty = set_atoms_ptr();
  return eval(*conjunction, empty);
}

} // namespace whitemech::lydia