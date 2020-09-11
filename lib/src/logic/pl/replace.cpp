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

#include <lydia/logic/pl/replace.hpp>
#include <utility>

namespace whitemech::lydia {

void ReplaceVisitor::visit(const PropositionalAtom &f) {
  basic_ptr basic = f.shared_from_this();
  prop_ptr prop = std::static_pointer_cast<const PropositionalFormula>(basic);
  if (replacements.find(prop) != replacements.end()) {
    result = replacements[prop];
  } else {
    result = prop;
  }
}

void ReplaceVisitor::visit(const PropositionalAnd &f) {
  set_prop_formulas new_args;
  for (const auto &subformula : f.get_container()) {
    new_args.insert(apply(*subformula));
  }
  result = logical_and(new_args);
}

void ReplaceVisitor::visit(const PropositionalOr &f) {
  set_prop_formulas new_args;
  for (const auto &subformula : f.get_container()) {
    new_args.insert(apply(*subformula));
  }
  result = logical_or(new_args);
}

void ReplaceVisitor::visit(const PropositionalNot &f) {
  result = apply(*f.get_arg())->logical_not();
}

prop_ptr ReplaceVisitor::apply(const PropositionalFormula &b) {
  b.accept(*this);
  return result;
}

void ReplaceVisitor::visit(const PropositionalTrue &) {
  result = boolean_prop(true);
}
void ReplaceVisitor::visit(const PropositionalFalse &) {
  result = boolean_prop(false);
}

prop_ptr replace(std::map<prop_ptr, prop_ptr, SharedComparator> replacements,
                 const PropositionalFormula &formula) {
  ReplaceVisitor visitor(std::move(replacements));
  return visitor.apply(formula);
}

} // namespace whitemech::lydia