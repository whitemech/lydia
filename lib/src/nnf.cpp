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

#include <lydia/logic/nnf.hpp>

namespace whitemech::lydia {

void NNFTransformer::visit(const LDLfTrue& x) {
  result = x.ctx().makeLdlfTrue();
}
void NNFTransformer::visit(const LDLfFalse& x) {
  result = x.ctx().makeLdlfFalse();
}

void NNFTransformer::visit(const LDLfAnd& x) {
  auto container = x.get_container();
  set_formulas new_container;
  for (auto& a : container) {
    new_container.insert(apply(*a));
  }
  result = x.ctx().makeLdlfAnd(new_container);
}

void NNFTransformer::visit(const LDLfOr& x) {
  auto container = x.get_container();
  set_formulas new_container;
  for (auto& a : container) {
    new_container.insert(apply(*a));
  }
  result = x.ctx().makeLdlfOr(new_container);
}

void NNFTransformer::visit(const LDLfNot& x) {
  auto new_formula = apply(*x.get_arg()->logical_not());
  result = new_formula;
}

void NNFTransformer::visit(const LDLfDiamond& x) {
  result =
      x.ctx().makeLdlfDiamond(apply(*x.get_regex()), apply(*x.get_formula()));
}

void NNFTransformer::visit(const LDLfBox& x) {
  result = x.ctx().makeLdlfBox(apply(*x.get_regex()), apply(*x.get_formula()));
}

void NNFTransformer::visit(const PropositionalRegExp& x) {
  regex_result = x.ctx().makePropRegex(apply(*x.get_arg()));
}

void NNFTransformer::visit(const TestRegExp& x) {
  regex_result = x.ctx().makeTestRegex(apply(*x.get_arg()));
}

void NNFTransformer::visit(const UnionRegExp& x) {
  auto container = x.get_container();
  set_regex new_container;
  for (auto& a : container) {
    new_container.insert(apply(*a));
  }
  regex_result = x.ctx().makeUnionRegex(new_container);
}

void NNFTransformer::visit(const SequenceRegExp& x) {
  auto container = x.get_container();
  vec_regex new_container;
  for (auto& a : container) {
    new_container.push_back(apply(*a));
  }
  regex_result = x.ctx().makeSeqRegex(new_container);
}

void NNFTransformer::visit(const StarRegExp& x) {
  regex_result = x.ctx().makeStarRegex(apply(*x.get_arg()));
}

void NNFTransformer::visit(const LDLfF& x) { result = apply(*x.get_arg()); }

void NNFTransformer::visit(const LDLfT& x) { result = apply(*x.get_arg()); }

void NNFTransformer::visit(const PropositionalTrue& f) {
  prop_result = f.ctx().makeTrue();
}
void NNFTransformer::visit(const PropositionalFalse& f) {
  prop_result = f.ctx().makeFalse();
}
void NNFTransformer::visit(const PropositionalAtom& f) {
  prop_result = f.ctx().makePropAtom(f.symbol);
}
void NNFTransformer::visit(const PropositionalAnd& f) {
  prop_result = f.ctx().makePropAnd(f.get_container());
}
void NNFTransformer::visit(const PropositionalOr& f) {
  prop_result = f.ctx().makePropOr(f.get_container());
}
void NNFTransformer::visit(const PropositionalNot& f) {
  if (is_a<PropositionalAtom>(*f.get_arg()))
    prop_result = f.ctx().makePropNot(f.get_arg());
  else
    prop_result = apply(*f.get_arg()->logical_not());
}

ldlf_ptr NNFTransformer::apply(const LDLfFormula& b) {
  b.accept(*this);
  return result;
}

regex_ptr NNFTransformer::apply(const RegExp& b) {
  b.accept(*this);
  return regex_result;
}

prop_ptr NNFTransformer::apply(const PropositionalFormula& b) {
  b.accept(*this);
  return prop_result;
}

std::shared_ptr<const LDLfFormula> to_nnf(const LDLfFormula& x) {
  NNFTransformer nnfTransformer;
  return nnfTransformer.apply(x);
}

} // namespace whitemech::lydia