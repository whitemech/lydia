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

Logger AtomsVisitor::logger = Logger("atom_visitor");

void AtomsVisitor::visit(const PropositionalTrue &) {}
void AtomsVisitor::visit(const PropositionalFalse &) {}

void AtomsVisitor::visit(const PropositionalAtom &x) {
  if (x.symbol->get_type_code() == TypeID::t_Symbol) {
    result.insert(std::make_shared<const PropositionalAtom>(
        dynamic_cast<const Symbol &>(*x.symbol)));
  } else if (x.symbol->get_type_code() == TypeID::t_QuotedFormula) {
    basic_ptr ptr = std::make_shared<const QuotedFormula>(
        dynamic_cast<const QuotedFormula &>(*x.symbol).formula);
    result.insert(std::make_shared<const PropositionalAtom>(ptr));
  } else {
    logger.error("Should not be here...");
    assert(false);
  }
}

void AtomsVisitor::visit(const PropositionalAnd &x) {
  for (auto &a : x.get_container()) {
    apply(*a);
  }
}

void AtomsVisitor::visit(const PropositionalOr &x) {
  for (auto &a : x.get_container()) {
    apply(*a);
  }
}

void AtomsVisitor::visit(const PropositionalNot &x) { apply(*x.get_arg()); }

// TODO merge the two temporal formulas in one single template,
//  and implement visit for each regex
void AtomsVisitor::visit(const LDLfDiamond<PropositionalRegExp> &f) {
  result = apply(*f.get_regex()->get_arg());
}

void AtomsVisitor::visit(const LDLfBox<PropositionalRegExp> &f) {
  result = apply(*f.get_regex()->get_arg());
}

set_atoms_ptr AtomsVisitor::apply(const PropositionalFormula &b) {
  b.accept(*this);
  return result;
}

set_atoms_ptr AtomsVisitor::apply(const LDLfFormula &b) {
  b.accept(*this);
  return result;
}

set_atoms_ptr find_atoms(const PropositionalFormula &f) {
  AtomsVisitor atomsVisitor;
  return atomsVisitor.apply(f);
}

set_atoms_ptr find_atoms(const LDLfFormula &f) {
  AtomsVisitor atomsVisitor;
  return atomsVisitor.apply(f);
}

} // namespace lydia
} // namespace whitemech