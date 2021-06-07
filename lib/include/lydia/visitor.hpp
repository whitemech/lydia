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

#include <lydia/basic.hpp>
#include <lydia/types.hpp>

namespace whitemech::lydia {

class Visitor {
public:
  // callbacks for LTLf
  virtual void visit(const LTLfTrue&){};
  virtual void visit(const LTLfFalse&){};
  virtual void visit(const LTLfAtom&){};
  virtual void visit(const LTLfAnd&){};
  virtual void visit(const LTLfOr&){};
  virtual void visit(const LTLfNot&){};
  virtual void visit(const LTLfNext&){};
  virtual void visit(const LTLfWeakNext&){};
  virtual void visit(const LTLfUntil&){};
  virtual void visit(const LTLfRelease&){};
  virtual void visit(const LTLfEventually&){};
  virtual void visit(const LTLfAlways&){};

  // callbacks for LDLf
  virtual void visit(const LDLfTrue&){};
  virtual void visit(const LDLfFalse&){};
  virtual void visit(const LDLfAnd&){};
  virtual void visit(const LDLfOr&){};
  virtual void visit(const LDLfNot&){};
  virtual void visit(const LDLfDiamond&){};
  virtual void visit(const LDLfBox&){};

  // callbacks for regular expressions
  virtual void visit(const PropositionalRegExp&){};
  virtual void visit(const TestRegExp&){};
  virtual void visit(const UnionRegExp&){};
  virtual void visit(const SequenceRegExp&){};
  virtual void visit(const StarRegExp&){};

  // callbacks for propositional logic
  virtual void visit(const PropositionalTrue&){};
  virtual void visit(const PropositionalFalse&){};
  virtual void visit(const PropositionalAtom&){};
  virtual void visit(const PropositionalAnd&){};
  virtual void visit(const PropositionalOr&){};
  virtual void visit(const PropositionalNot&){};

  virtual void visit(const Symbol&){};
  virtual void visit(const QuotedFormula&){};
  virtual void visit(const LDLfF&){};
  virtual void visit(const LDLfT&){};
  virtual void visit(const LDLfQ&){};
};

} // namespace whitemech::lydia
