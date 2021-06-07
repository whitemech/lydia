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
#include <lydia/logic/pl/models/base.hpp>
#include <lydia/utils/misc.hpp>

namespace whitemech::lydia {

class NaiveModelEnumerationStategy : public ModelEnumerationStrategy {

public:
  std::vector<set_atoms_ptr> all_models(const PropositionalFormula& f) {
    std::vector<set_atoms_ptr> models;
    auto all_atoms = find_atoms(f);
    std::vector<set_atoms_ptr> all_interpretations =
        powerset<atom_ptr, SharedComparator>(all_atoms);
    for (set_atoms_ptr& interpretation : all_interpretations) {
      if (eval(f, interpretation)) {
        models.push_back(interpretation);
      }
    }
    return models;
  }
};

} // namespace whitemech::lydia