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

#include "delta.hpp"
#include "visitor.hpp"
#include <variant>

namespace whitemech {
namespace lydia {

class AtomsVisitor : public Visitor {
private:
protected:
  set_atoms_ptr result;

public:
  static Logger logger;

  // callbacks for LDLf
  void visit(const Symbol &) override{};
  void visit(const LDLfBooleanAtom &) override{};
  void visit(const LDLfAnd &) override{};
  void visit(const LDLfOr &) override{};
  void visit(const LDLfNot &) override{};
  void visit(const LDLfDiamond<PropositionalRegExp> &x) override {
    this->visit_temporal(x);
  };
  void visit(const LDLfDiamond<TestRegExp> &x) override {
    this->visit_temporal(x);
  };
  void visit(const LDLfBox<PropositionalRegExp> &x) override {
    this->visit_temporal(x);
  };
  void visit(const LDLfBox<TestRegExp> &x) override {
    this->visit_temporal(x);
  };

  // callbacks for regular expressions
  void visit(const PropositionalRegExp &) override;
  void visit(const TestRegExp &) override;

  // callbacks for propositional logic
  void visit(const PropositionalTrue &) override;
  void visit(const PropositionalFalse &) override;
  void visit(const PropositionalAtom &) override;
  void visit(const PropositionalAnd &) override;
  void visit(const PropositionalOr &) override;
  void visit(const PropositionalNot &) override;

  void visit(const QuotedFormula &) override{};

  template <class R> void inline visit_temporal(const LDLfTemporal<R> &x) {
    result = apply(*x.get_regex());
    auto y = apply(*x.get_formula());
    result.insert(y.begin(), y.end());
  }

  set_atoms_ptr apply(const PropositionalFormula &b);
  set_atoms_ptr apply(const LDLfFormula &b);
  set_atoms_ptr apply(const RegExp &b);
};

set_atoms_ptr find_atoms(const LDLfFormula &);
set_atoms_ptr find_atoms(const PropositionalFormula &);

} // namespace lydia
} // namespace whitemech