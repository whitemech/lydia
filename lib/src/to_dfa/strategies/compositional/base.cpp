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

#include <lydia/atom_visitor.hpp>
#include <lydia/nnf.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>

namespace whitemech {
namespace lydia {

std::shared_ptr<abstract_dfa>
CompositionalStrategy::to_dfa(const LDLfFormula &formula) {
  auto formula_nnf = to_nnf(formula);
  set_atoms_ptr atoms = find_atoms(*formula_nnf);
  int index = 0;
  for (const auto &atom : atoms) {
    atom2ids[atom] = index;
    id2atoms.push_back(atom);
    index++;
  }

  auto visitor = ComposeDFAVisitor(*this);
  auto result = visitor.apply(formula);
  result = dfaLDLfTrue();
  return std::make_shared<mona_dfa>(result);
}

} // namespace lydia
} // namespace whitemech