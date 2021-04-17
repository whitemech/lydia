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
#include <lydia/logic/pl/base.hpp>
#include <lydia/visitor.hpp>

namespace whitemech::lydia {

class NNFTransformer : public Visitor {
private:
protected:
  // TODO split into several transformers?
  ldlf_ptr result;
  regex_ptr regex_result;
  prop_ptr prop_result;

public:
  // callbacks for LDLf
  void visit(const Symbol &) override{};
  void visit(const LDLfTrue &) override;
  void visit(const LDLfFalse &) override;
  void visit(const LDLfAnd &) override;
  void visit(const LDLfOr &) override;
  void visit(const LDLfNot &) override;
  void visit(const LDLfDiamond &x) override;
  void visit(const LDLfBox &x) override;

  // callbacks for regular expressions
  void visit(const PropositionalRegExp &) override;
  void visit(const TestRegExp &) override;
  void visit(const UnionRegExp &) override;
  void visit(const SequenceRegExp &) override;
  void visit(const StarRegExp &) override;

  // callbacks for propositional logic
  void visit(const PropositionalTrue &) override;
  void visit(const PropositionalFalse &) override;
  void visit(const PropositionalAtom &) override;
  void visit(const PropositionalAnd &) override;
  void visit(const PropositionalOr &) override;
  void visit(const PropositionalNot &) override;

  void visit(const QuotedFormula &) override{};
  void visit(const LDLfF &) override;
  void visit(const LDLfT &) override;

  ldlf_ptr apply(const LDLfFormula &b);
  regex_ptr apply(const RegExp &b);
  prop_ptr apply(const PropositionalFormula &b);
};

std::shared_ptr<const LDLfFormula> to_nnf(const LDLfFormula &);

} // namespace whitemech::lydia