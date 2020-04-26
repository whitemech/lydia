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

#include "logic.hpp"
#include "nnf.hpp"
#include "propositional_logic.hpp"
#include "visitor.hpp"

namespace whitemech {
namespace lydia {

class StrPrinter : public Visitor {
private:
  static const std::vector<std::string> names_;

protected:
  std::string result;

public:
  // callbacks for LDLf
  void visit(const Symbol &) override;
  void visit(const LDLfBooleanAtom &) override;
  void visit(const LDLfAnd &) override;
  void visit(const LDLfOr &) override;
  void visit(const LDLfNot &) override;
  void visit(const LDLfDiamond<PropositionalRegExp> &) override{};
  void visit(const LDLfDiamond<TestRegExp> &) override{};
  void visit(const LDLfBox<PropositionalRegExp> &) override{};
  void visit(const LDLfBox<TestRegExp> &) override{};
  // TODO add all the combinations of temporal formulas + regular expression

  // callbacks for regular expressions
  void visit(const PropositionalRegExp &) override{};
  void visit(const TestRegExp &) override{};

  // callbacks for propositional logic
  void visit(const PropositionalTrue &) override{};
  void visit(const PropositionalFalse &) override{};
  void visit(const PropositionalAtom &) override{};
  void visit(const PropositionalAnd &) override{};
  void visit(const PropositionalOr &) override{};
  void visit(const PropositionalNot &) override{};

  void visit(const QuotedFormula &) override{};

  template <class T> void visit(const LDLfDiamond<T> &);

  std::string apply(const vec_basic &v);
  std::string apply(const set_formulas &v);
  std::string apply(const Basic &b);
};

std::string to_string(const Basic &);

} // namespace lydia
} // namespace whitemech