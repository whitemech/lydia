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
#include <lydia/logic/pl/base.hpp>
#include <lydia/logic/pl/eval.hpp>

namespace whitemech::lydia {

void EvalVisitor::visit(const PropositionalTrue&) { result = true; }
void EvalVisitor::visit(const PropositionalFalse&) { result = false; }

void EvalVisitor::visit(const PropositionalAtom& a) {
  for (const auto& x : interpretation) {
    if (*x == a) {
      result = true;
      return;
    }
  }
  result = false;
}

void EvalVisitor::visit(const PropositionalNot& a) {
  result = not apply(*a.get_arg());
}

void EvalVisitor::visit(const PropositionalAnd& a) {
  result = true;
  for (const auto& subformula : a.get_container()) {
    if (!apply(*subformula)) {
      result = false;
      return;
    }
  }
}

void EvalVisitor::visit(const PropositionalOr& a) {
  result = false;
  for (const auto& subformula : a.get_container()) {
    if (apply(*subformula)) {
      result = true;
      return;
    }
  }
}

bool EvalVisitor::apply(const PropositionalFormula& b) {
  b.accept(*this);
  return result;
}

bool eval(const PropositionalFormula& f, const set_atoms_ptr& interpretation) {
  EvalVisitor evalVisitor{interpretation};
  return evalVisitor.apply(f);
}

} // namespace whitemech::lydia
