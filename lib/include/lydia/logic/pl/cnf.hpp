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
#include <lydia/visitor.hpp>

namespace whitemech::lydia {

class CNFTransformer : public Visitor {
private:
protected:
  std::shared_ptr<const PropositionalFormula> result;

public:
  // callbacks for propositional logic
  void visit(const PropositionalTrue&) override;
  void visit(const PropositionalFalse&) override;
  void visit(const PropositionalAtom&) override;
  void visit(const PropositionalAnd&) override;
  void visit(const PropositionalOr&) override;
  void visit(const PropositionalNot&) override;

  void visit(const QuotedFormula&) override{};
  void visit(const LDLfF&) override{};
  void visit(const LDLfT&) override{};

  prop_ptr apply(const PropositionalFormula& b);
};

set_prop_formulas to_container(prop_ptr p);
prop_ptr to_cnf(const PropositionalFormula&);

std::vector<std::vector<PropositionalAtom>>
to_clauses(const PropositionalFormula& cnf_f);

} // namespace whitemech::lydia