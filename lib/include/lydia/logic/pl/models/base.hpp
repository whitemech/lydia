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

#include <lydia/logic/atom_visitor.hpp>
#include <lydia/logic/pl/cnf.hpp>
#include <lydia/logic/pl/eval.hpp>
#include <lydia/utils/misc.hpp>

namespace whitemech::lydia {

/*
 * Functor to enumerate the models of a
 * propositional formula.
 */
class ModelEnumerationStrategy {

public:
  /*!
   * Compute all the models of a propositional formula.
   *
   * @param f the propositional formula
   * @return the set of the models of a formula
   */
  virtual std::vector<set_atoms_ptr>
  all_models(const PropositionalFormula& f) = 0;
};

template <class T>
std::vector<set_atoms_ptr> all_models(const PropositionalFormula& f) {
  T s = T();
  return s.all_models(f);
}

} // namespace whitemech::lydia