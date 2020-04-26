#pragma once
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

#include "basic.hpp"
#include "types.hpp"

namespace whitemech {
namespace lydia {

class Visitor {
public:
  // callbacks for LDLf
  virtual void visit(const Symbol &) = 0;
  virtual void visit(const LDLfBooleanAtom &) = 0;
  virtual void visit(const LDLfAnd &) = 0;
  virtual void visit(const LDLfOr &) = 0;
  virtual void visit(const LDLfNot &) = 0;
  virtual void visit(const LDLfDiamond<PropositionalRegExp> &) = 0;
  virtual void visit(const LDLfDiamond<TestRegExp> &) = 0;
  virtual void visit(const LDLfBox<PropositionalRegExp> &) = 0;
  virtual void visit(const LDLfBox<TestRegExp> &) = 0;
  // TODO add all the combinations of temporal formulas + regular expression

  // callbacks for regular expressions
  virtual void visit(const PropositionalRegExp &) = 0;
  virtual void visit(const TestRegExp &) = 0;

  // callbacks for propositional logic
  virtual void visit(const PropositionalTrue &) = 0;
  virtual void visit(const PropositionalFalse &) = 0;
  virtual void visit(const PropositionalAtom &) = 0;
  virtual void visit(const PropositionalAnd &) = 0;
  virtual void visit(const PropositionalOr &) = 0;
  virtual void visit(const PropositionalNot &) = 0;

  virtual void visit(const QuotedFormula &) = 0;
};

} // namespace lydia
} // namespace whitemech
