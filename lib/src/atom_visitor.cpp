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
  set_atoms_ptr atoms_result;
  if (x.symbol->get_type_code() == TypeID::t_Symbol) {
    atoms_result.insert(std::make_shared<const PropositionalAtom>(
        dynamic_cast<const Symbol &>(*x.symbol)));
  } else if (x.symbol->get_type_code() == TypeID::t_QuotedFormula) {
    basic_ptr ptr = std::make_shared<const QuotedFormula>(
        dynamic_cast<const QuotedFormula &>(*x.symbol).formula);
    atoms_result.insert(std::make_shared<const PropositionalAtom>(ptr));
  } else {
    logger.error("Should not be here...");
    assert(false);
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const PropositionalAnd &x) {
  set_atoms_ptr atoms_result, tmp;
  for (auto &a : x.get_container()) {
    tmp = apply(*a);
    atoms_result.insert(tmp.begin(), tmp.end());
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const PropositionalOr &x) {
  set_atoms_ptr atoms_result, tmp;
  for (auto &a : x.get_container()) {
    tmp = apply(*a);
    atoms_result.insert(tmp.begin(), tmp.end());
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const PropositionalNot &x) {
  result = apply(*x.get_arg());
}

// LDLf

void AtomsVisitor::visit(const LDLfBooleanAtom &) {}

void AtomsVisitor::visit(const LDLfAnd &x) {
  set_atoms_ptr atoms_result, tmp;
  for (auto &a : x.get_container()) {
    tmp = apply(*a);
    atoms_result.insert(tmp.begin(), tmp.end());
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const LDLfOr &x) {
  set_atoms_ptr atoms_result, tmp;
  for (auto &a : x.get_container()) {
    tmp = apply(*a);
    atoms_result.insert(tmp.begin(), tmp.end());
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const LDLfNot &x) { result = apply(*x.get_arg()); }

void AtomsVisitor::visit(const LDLfDiamond &x) {
  result = apply(*x.get_regex());
  auto y = apply(*x.get_formula());
  result.insert(y.begin(), y.end());
}

void AtomsVisitor::visit(const LDLfBox &x) {
  result = apply(*x.get_regex());
  auto y = apply(*x.get_formula());
  result.insert(y.begin(), y.end());
}

void AtomsVisitor::visit(const UnionRegExp &x) {
  set_atoms_ptr atoms_result, tmp;
  for (auto &a : x.get_container()) {
    tmp = apply(*a);
    atoms_result.insert(tmp.begin(), tmp.end());
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const SequenceRegExp &x) {
  set_atoms_ptr atoms_result, tmp;
  for (auto &a : x.get_container()) {
    tmp = apply(*a);
    atoms_result.insert(tmp.begin(), tmp.end());
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const StarRegExp &x) { result = apply(*x.get_arg()); }

void AtomsVisitor::visit(const PropositionalRegExp &r) {
  result = apply(*r.get_arg());
}

void AtomsVisitor::visit(const TestRegExp &r) { result = apply(*r.get_arg()); }
set_atoms_ptr AtomsVisitor::apply(const RegExp &b) {
  b.accept(*this);
  return result;
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