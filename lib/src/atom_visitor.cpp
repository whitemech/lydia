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

namespace whitemech::lydia {

Logger AtomsVisitor::logger = Logger("atom_visitor");

void AtomsVisitor::visit(const PropositionalTrue&) {}
void AtomsVisitor::visit(const PropositionalFalse&) {}

void AtomsVisitor::visit(const PropositionalAtom& x) {
  set_atoms_ptr atoms_result;
  if (auto s = std::dynamic_pointer_cast<const Symbol>(x.symbol)) {
    auto atom = x.ctx().makePropAtom(s->get_name());
    atoms_result.insert(
        std::static_pointer_cast<const PropositionalAtom>(atom));
  } else if (auto q =
                 std::dynamic_pointer_cast<const QuotedFormula>(x.symbol)) {
    auto atom = x.ctx().makePropAtom(quote(q->formula));
    atoms_result.insert(
        std::static_pointer_cast<const PropositionalAtom>(atom));
  } else {
    logger.error("Should not be here...");
    assert(false);
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const PropositionalAnd& x) {
  set_atoms_ptr atoms_result, tmp;
  for (auto& a : x.get_container()) {
    tmp = apply(*a);
    atoms_result.insert(tmp.begin(), tmp.end());
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const PropositionalOr& x) {
  set_atoms_ptr atoms_result, tmp;
  for (auto& a : x.get_container()) {
    tmp = apply(*a);
    atoms_result.insert(tmp.begin(), tmp.end());
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const PropositionalNot& x) {
  result = apply(*x.get_arg());
}

// LDLf

void AtomsVisitor::visit(const LDLfTrue&) {}
void AtomsVisitor::visit(const LDLfFalse&) {}

void AtomsVisitor::visit(const LDLfAnd& x) {
  set_atoms_ptr atoms_result, tmp;
  for (auto& a : x.get_container()) {
    tmp = apply(*a);
    atoms_result.insert(tmp.begin(), tmp.end());
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const LDLfOr& x) {
  set_atoms_ptr atoms_result, tmp;
  for (auto& a : x.get_container()) {
    tmp = apply(*a);
    atoms_result.insert(tmp.begin(), tmp.end());
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const LDLfNot& x) { result = apply(*x.get_arg()); }

void AtomsVisitor::visit(const LDLfDiamond& x) {
  auto tmp = apply(*x.get_regex());
  auto y = apply(*x.get_formula());
  tmp.insert(y.begin(), y.end());
  result = tmp;
}

void AtomsVisitor::visit(const LDLfBox& x) {
  auto tmp = apply(*x.get_regex());
  auto y = apply(*x.get_formula());
  tmp.insert(y.begin(), y.end());
  result = tmp;
}

void AtomsVisitor::visit(const UnionRegExp& x) {
  set_atoms_ptr atoms_result, tmp;
  for (auto& a : x.get_container()) {
    tmp = apply(*a);
    atoms_result.insert(tmp.begin(), tmp.end());
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const SequenceRegExp& x) {
  set_atoms_ptr atoms_result, tmp;
  for (auto& a : x.get_container()) {
    tmp = apply(*a);
    atoms_result.insert(tmp.begin(), tmp.end());
  }
  result = atoms_result;
}

void AtomsVisitor::visit(const StarRegExp& x) { result = apply(*x.get_arg()); }

void AtomsVisitor::visit(const PropositionalRegExp& r) {
  result = apply(*r.get_arg());
}

void AtomsVisitor::visit(const TestRegExp& r) { result = apply(*r.get_arg()); }
set_atoms_ptr AtomsVisitor::apply(const RegExp& b) {
  b.accept(*this);
  return result;
}

set_atoms_ptr AtomsVisitor::apply(const PropositionalFormula& b) {
  b.accept(*this);
  return result;
}

set_atoms_ptr AtomsVisitor::apply(const LDLfFormula& b) {
  b.accept(*this);
  return result;
}

set_atoms_ptr find_atoms(const PropositionalFormula& f) {
  AtomsVisitor atomsVisitor;
  return atomsVisitor.apply(f);
}

set_atoms_ptr find_atoms(const LDLfFormula& f) {
  AtomsVisitor atomsVisitor;
  return atomsVisitor.apply(f);
}

} // namespace whitemech::lydia