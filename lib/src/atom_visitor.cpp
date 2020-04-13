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

#include <atom_visitor.hpp>

namespace whitemech {
namespace lydia {

void AtomsVisitor::visit(const PropositionalTrue &) {}
void AtomsVisitor::visit(const PropositionalFalse &) {}

void AtomsVisitor::visit(const PropositionalAtom &x) {
  result.insert(std::make_shared<PropositionalAtom>(x.symbol));
}

void AtomsVisitor::visit(const QuotedFormula &) {}

void AtomsVisitor::visit(const PropositionalAnd &x) {
  for (auto &a : x.get_container()) {
    auto atoms = apply(*a);
    result.insert(atoms.begin(), atoms.end());
  }
}

void AtomsVisitor::visit(const PropositionalOr &x) {
  for (auto &a : x.get_container()) {
    auto atoms = apply(*a);
    result.insert(atoms.begin(), atoms.end());
  }
}

void AtomsVisitor::visit(const PropositionalNot &x) {
  auto tmp = apply(*x.get_arg());
  result.insert(tmp.begin(), tmp.end());
}

std::set<std::shared_ptr<PropositionalAtom>>
AtomsVisitor::apply(const PropositionalFormula &b) {
  b.accept(*this);
  return result;
}

std::set<std::shared_ptr<PropositionalAtom>>
find_atoms(const PropositionalFormula &f) {
  AtomsVisitor atomsVisitor;
  return atomsVisitor.apply(f);
}

} // namespace lydia
} // namespace whitemech