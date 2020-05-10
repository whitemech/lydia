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

#include <cryptominisat5/cryptominisat.h>
#include <lydia/atom_visitor.hpp>
#include <lydia/pl/eval.hpp>
#include <lydia/utils/misc.hpp>

namespace whitemech {
namespace lydia {

/*!
 * Compute all the models of a propositional formula.
 *
 * @param f the propositional formula
 * @return the set of the models of a formula
 */
std::vector<set_atoms_ptr> all_models(const PropositionalFormula &f);

/*!
 * Compute the minimal models of a propositional formula.
 *
 * @param f the propositional formula
 * @return the set of minimal models.
 */
std::vector<set_atoms_ptr> minimal_models(const PropositionalFormula &f);

} // namespace lydia
} // namespace whitemech