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
#include "propositional_logic.hpp"
#include "visitor.hpp"

namespace whitemech {
namespace lydia {

class NNFTransformer : public Visitor {
private:
protected:
  // TODO split into several transformers?
  std::shared_ptr<const LDLfFormula> result;
  std::shared_ptr<const RegExp> regex_result;

public:
  // callbacks for LDLf
  void visit(const Symbol &) override{};
  void visit(const LDLfBooleanAtom &) override;
  void visit(const LDLfAnd &) override;
  void visit(const LDLfOr &) override;
  void visit(const LDLfNot &) override;
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
  void visit(const PropositionalTrue &) override{};
  void visit(const PropositionalFalse &) override{};
  void visit(const PropositionalAtom &) override{};
  void visit(const PropositionalAnd &) override{};
  void visit(const PropositionalOr &) override{};
  void visit(const PropositionalNot &) override{};

  void visit(const QuotedFormula &) override{};

  std::shared_ptr<const LDLfFormula> apply(const LDLfFormula &b);
  std::shared_ptr<const RegExp> apply(const RegExp &b);

  template <typename T, typename = typename std::enable_if<
                            std::is_base_of<RegExp, T>::value, T>::type>
  void inline visit_temporal(const LDLfTemporal<T> &x) {
    if (x.type_code_ == TypeID::t_LDLfDiamond) {
      result = std::make_shared<LDLfDiamond<T>>(apply(*x.get_regex()),
                                                apply(*x.get_formula()));
    } else if (x.type_code_ == TypeID::t_LDLfBox) {
      result = std::make_shared<LDLfBox<T>>(apply(*x.get_regex()),
                                            apply(*x.get_formula()));
    }
  }
};

std::shared_ptr<const LDLfFormula> to_nnf(const LDLfFormula &);

} // namespace lydia
} // namespace whitemech