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

#include <cassert>
#include <delta.hpp>
#include <logger.hpp>

namespace whitemech {
namespace lydia {

Logger DeltaVisitor::logger = Logger("delta");

void DeltaVisitor::visit(const LDLfBooleanAtom &x) {
  if (x.get_value())
    result = std::make_shared<PropositionalTrue>();
  else
    result = std::make_shared<PropositionalFalse>();
}

void DeltaVisitor::visit(const LDLfNot &b) {
  DeltaVisitor::logger.error("Delta function should not be called with a not.");
  assert(false);
}

void DeltaVisitor::visit(const LDLfAnd &x) {
  auto container = x.get_container();
  set_prop_formulas new_container;
  for (auto &a : container) {
    new_container.insert(apply(*a));
  }
  result = std::make_shared<PropositionalAnd>(new_container);
}

void DeltaVisitor::visit(const LDLfOr &x) {
  auto container = x.get_container();
  set_prop_formulas new_container;
  for (auto &a : container) {
    new_container.insert(apply(*a));
  }
  result = std::make_shared<PropositionalOr>(new_container);
}

std::shared_ptr<const PropositionalFormula>
DeltaVisitor::apply(const LDLfFormula &b) {
  b.accept(*this);
  return result;
}

std::shared_ptr<const PropositionalFormula> delta(const LDLfFormula &x,
                                                  bool epsilon) {
  DeltaVisitor deltaVisitor{epsilon};
  return deltaVisitor.apply(x);
}

} // namespace lydia
} // namespace whitemech