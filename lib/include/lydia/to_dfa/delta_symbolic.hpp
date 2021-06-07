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

#include <cuddObj.hh>
#include <lydia/logger.hpp>
#include <lydia/logic/ldlf/base.hpp>
#include <lydia/logic/pl/base.hpp>
#include <lydia/to_dfa/delta.hpp>
#include <lydia/visitor.hpp>
#include <utility>

namespace whitemech::lydia {

class DeltaSymbolicVisitor : public Visitor {
private:
protected:
  std::shared_ptr<const PropositionalFormula> result;
  bool epsilon;

public:
  static Logger logger;
  DeltaSymbolicVisitor() : epsilon{true} {}
  explicit DeltaSymbolicVisitor(bool epsilon = false) : epsilon{epsilon} {}

  // callbacks for LDLf
  void visit(const Symbol&) override{};
  void visit(const LDLfTrue&) override;
  void visit(const LDLfFalse&) override;
  void visit(const LDLfAnd&) override;
  void visit(const LDLfOr&) override;
  void visit(const LDLfNot&) override;
  void visit(const LDLfDiamond&) override;
  void visit(const LDLfBox&) override;

  // callbacks for regular expressions
  void visit(const PropositionalRegExp&) override{};
  void visit(const TestRegExp&) override{};
  void visit(const UnionRegExp&) override{};
  void visit(const SequenceRegExp&) override{};
  void visit(const StarRegExp&) override{};

  // callbacks for propositional logic
  void visit(const PropositionalTrue&) override{};
  void visit(const PropositionalFalse&) override{};
  void visit(const PropositionalAtom&) override{};
  void visit(const PropositionalAnd&) override{};
  void visit(const PropositionalOr&) override{};
  void visit(const PropositionalNot&) override{};

  void visit(const QuotedFormula&) override{};
  void visit(const LDLfF&) override;
  void visit(const LDLfT&) override;

  std::shared_ptr<const PropositionalFormula> apply(const LDLfFormula& b);
};

class DeltaSymbolicDiamondRegExpVisitor : Visitor {
protected:
  std::shared_ptr<const PropositionalFormula> result;
  const LDLfTemporal& formula;
  bool epsilon;

public:
  explicit DeltaSymbolicDiamondRegExpVisitor(const LDLfTemporal& formula,
                                             bool epsilon)
      : formula{std::move(formula)}, epsilon{epsilon} {};

  // callbacks for regular expressions
  void visit(const PropositionalRegExp&) override;
  void visit(const TestRegExp&) override;
  void visit(const UnionRegExp&) override;
  void visit(const SequenceRegExp&) override;
  void visit(const StarRegExp&) override;

  std::shared_ptr<const PropositionalFormula> apply(const RegExp& b);
};

class DeltaSymbolicBoxRegExpVisitor : Visitor {
protected:
  std::shared_ptr<const PropositionalFormula> result;
  const LDLfTemporal& formula;
  const set_atoms_ptr prop_interpretation;
  bool epsilon;

public:
  explicit DeltaSymbolicBoxRegExpVisitor(const LDLfTemporal& formula,
                                         bool epsilon)
      : formula{std::move(formula)}, epsilon{epsilon} {};
  // callbacks for regular expressions
  void visit(const PropositionalRegExp&) override;
  void visit(const TestRegExp&) override;
  void visit(const UnionRegExp&) override;
  void visit(const SequenceRegExp&) override;
  void visit(const StarRegExp&) override;
  std::shared_ptr<const PropositionalFormula> apply(const RegExp& b);
};

std::shared_ptr<const PropositionalFormula>
delta_symbolic(const LDLfFormula&, bool epsilon = false);

} // namespace whitemech::lydia