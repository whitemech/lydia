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

#include <lydia/logic/pl/base.hpp>
#include <lydia/visitor.hpp>
#include <utility>

namespace whitemech::lydia {

class ReplaceVisitor : public Visitor {
private:
  prop_ptr result;
  std::map<prop_ptr, prop_ptr, SharedComparator> replacements;

public:
  explicit ReplaceVisitor(
      std::map<prop_ptr, prop_ptr, SharedComparator> replacements)
      : replacements{std::move(replacements)} {}
  // callbacks for propositional logic
  void visit(const PropositionalTrue&) override;
  void visit(const PropositionalFalse&) override;
  void visit(const PropositionalAtom&) override;
  void visit(const PropositionalAnd&) override;
  void visit(const PropositionalOr&) override;
  void visit(const PropositionalNot&) override;

  prop_ptr apply(const PropositionalFormula& b);
};

prop_ptr replace(std::map<prop_ptr, prop_ptr, SharedComparator> replacements,
                 const PropositionalFormula& formula);

} // namespace whitemech::lydia