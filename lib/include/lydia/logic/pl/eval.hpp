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

#include <lydia/logic/ldlf/base.hpp>

namespace whitemech::lydia {

class EvalVisitor : public Visitor {
private:
protected:
  bool result{};
  const set_atoms_ptr& interpretation;

public:
  explicit EvalVisitor(const set_atoms_ptr& interpretation)
      : interpretation{interpretation} {};

  // callbacks for propositional logic
  void visit(const PropositionalTrue&) override;
  void visit(const PropositionalFalse&) override;
  void visit(const PropositionalAtom&) override;
  void visit(const PropositionalAnd&) override;
  void visit(const PropositionalOr&) override;
  void visit(const PropositionalNot&) override;

  void visit(const QuotedFormula&) override{};

  bool apply(const PropositionalFormula& b);
};

/*!
 * Evaluate a formula.
 *
 * @param interpretation the set of atoms that are true in the interpretation.
 *                     | Atoms that are not members are considered false.
 * @return true if the formula is true in the interpretation, false otherwise.
 */
bool eval(const PropositionalFormula&, const set_atoms_ptr& interpretation);

} // namespace whitemech::lydia