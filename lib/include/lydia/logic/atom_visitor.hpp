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

#include <lydia/to_dfa/delta.hpp>
#include <lydia/visitor.hpp>
#include <variant>

namespace whitemech::lydia {

class AtomsVisitor : public Visitor {
private:
protected:
  set_atoms_ptr result;

public:
  static Logger logger;

  // callbacks for LDLf
  void visit(const LDLfTrue&) override;
  void visit(const LDLfFalse&) override;
  void visit(const LDLfAnd&) override;
  void visit(const LDLfOr&) override;
  void visit(const LDLfNot&) override;
  void visit(const LDLfDiamond& x) override;
  void visit(const LDLfBox& x) override;

  // callbacks for regular expressions
  void visit(const PropositionalRegExp&) override;
  void visit(const TestRegExp&) override;
  void visit(const UnionRegExp&) override;
  void visit(const SequenceRegExp&) override;
  void visit(const StarRegExp&) override;

  // callbacks for propositional logic
  void visit(const PropositionalTrue&) override;
  void visit(const PropositionalFalse&) override;
  void visit(const PropositionalAtom&) override;
  void visit(const PropositionalAnd&) override;
  void visit(const PropositionalOr&) override;
  void visit(const PropositionalNot&) override;

  void visit(const QuotedFormula&) override{};
  void visit(const Symbol&) override{};

  set_atoms_ptr apply(const PropositionalFormula& b);
  set_atoms_ptr apply(const LDLfFormula& b);
  set_atoms_ptr apply(const RegExp& b);
};

set_atoms_ptr find_atoms(const LDLfFormula&);
set_atoms_ptr find_atoms(const PropositionalFormula&);

} // namespace whitemech::lydia
