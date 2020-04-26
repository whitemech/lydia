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

#include "nnf.hpp"

namespace whitemech {
namespace lydia {

void NNFTransformer::visit(const LDLfBooleanAtom &x) {
  result = std::make_shared<LDLfBooleanAtom>(x.get_value());
}

void NNFTransformer::visit(const LDLfAnd &x) {
  auto container = x.get_container();
  set_formulas new_container;
  for (auto &a : container) {
    new_container.insert(apply(*a));
  }
  result = std::make_shared<LDLfAnd>(new_container);
}

void NNFTransformer::visit(const LDLfOr &x) {
  auto container = x.get_container();
  set_formulas new_container;
  for (auto &a : container) {
    new_container.insert(apply(*a));
  }
  result = std::make_shared<LDLfOr>(new_container);
}

void NNFTransformer::visit(const LDLfNot &x) {
  auto new_formula = apply(*x.get_arg()->logical_not());
  result = new_formula;
}

std::shared_ptr<const LDLfFormula> NNFTransformer::apply(const LDLfFormula &b) {
  b.accept(*this);
  return result;
}

std::shared_ptr<const RegExp> NNFTransformer::apply(const RegExp &b) {
  b.accept(*this);
  return regex_result;
}

void NNFTransformer::visit(const PropositionalRegExp &x) {
  regex_result = std::make_shared<PropositionalRegExp>(x.get_arg());
}

void NNFTransformer::visit(const TestRegExp &x) {
  regex_result = std::make_shared<TestRegExp>(apply(*x.get_arg()));
}

std::shared_ptr<const LDLfFormula> to_nnf(const LDLfFormula &x) {
  NNFTransformer nnfTransformer;
  return nnfTransformer.apply(x);
}

} // namespace lydia
} // namespace whitemech