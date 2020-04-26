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

#include <cassert>
#include <delta.hpp>
#include <logger.hpp>
#include <nnf.hpp>

namespace whitemech {
namespace lydia {

Logger DeltaVisitor::logger = Logger("delta");

void DeltaVisitor::visit(const LDLfBooleanAtom &x) {
  if (x.get_value())
    result = std::make_shared<PropositionalTrue>();
  else
    result = std::make_shared<PropositionalFalse>();
}

void DeltaVisitor::visit(const LDLfNot &b) {
  DeltaVisitor::logger.error("Delta function should not be called with a not.");
  assert(false);
}

void DeltaVisitor::visit(const LDLfAnd &x) {
  auto container = x.get_container();
  set_prop_formulas new_container;
  for (auto &a : container) {
    new_container.insert(apply(*a));
  }
  result = std::make_shared<PropositionalAnd>(new_container);
}

void DeltaVisitor::visit(const LDLfOr &x) {
  auto container = x.get_container();
  set_prop_formulas new_container;
  for (auto &a : container) {
    new_container.insert(apply(*a));
  }
  result = std::make_shared<PropositionalOr>(new_container);
}

void DeltaVisitor::visit(const LDLfDiamond &f) {
  auto d = DeltaDiamondRegExpVisitor(prop_interpretation, f, epsilon);
  result = d.apply(*f.get_regex());
}

void DeltaVisitor::visit(const LDLfBox &f) {
  auto d = DeltaBoxRegExpVisitor(prop_interpretation, f, epsilon);
  result = d.apply(*f.get_regex());
}

std::shared_ptr<const PropositionalFormula>
DeltaVisitor::apply(const LDLfFormula &b) {
  b.accept(*this);
  return result;
}

std::shared_ptr<const PropositionalFormula> delta(const LDLfFormula &x) {
  // epsilon = true
  DeltaVisitor deltaVisitor;
  return deltaVisitor.apply(x);
}

std::shared_ptr<const PropositionalFormula> delta(const LDLfFormula &x,
                                                  const set_atoms_ptr &i) {
  DeltaVisitor deltaVisitor{i};
  return deltaVisitor.apply(x);
}

void DeltaDiamondRegExpVisitor::visit(const PropositionalRegExp &r) {
  if (epsilon) {
    result = std::make_shared<PropositionalFalse>();
    return;
  }
  auto prop = r.get_arg();

  if (eval(*prop, this->prop_interpretation)) {
    // TODO Implement the "E(phi)" in the delta function (Brafman et. al 2018)
    // TODO quote(formula) with formula as reference is potentially dangerous
    result = std::make_shared<PropositionalAtom>(quote(formula.get_formula()));
  } else {
    result = std::make_shared<PropositionalFalse>();
  }
}

void DeltaDiamondRegExpVisitor::visit(const TestRegExp &r) {
  auto d = DeltaVisitor(prop_interpretation, epsilon);
  auto regex_delta = d.apply(*r.get_arg());
  auto ldlf_delta = d.apply(*formula.get_formula());
  result = std::make_shared<PropositionalAnd>(
      set_prop_formulas{regex_delta, ldlf_delta});
}

void DeltaDiamondRegExpVisitor::visit(const UnionRegExp &r) {
  set_prop_formulas args;
  for (const auto &regex : r.get_container()) {
    auto new_f = LDLfDiamond(regex, formula.get_formula());
    auto tmp = DeltaVisitor(prop_interpretation, epsilon).apply(new_f);
    args.insert(tmp);
  }
  result = std::make_shared<PropositionalOr>(args);
}

void DeltaDiamondRegExpVisitor::visit(const SequenceRegExp &) {}

void DeltaDiamondRegExpVisitor::visit(const StarRegExp &) {}

std::shared_ptr<const PropositionalFormula>
DeltaDiamondRegExpVisitor::apply(const RegExp &b) {
  b.accept(*this);
  return result;
}

void DeltaBoxRegExpVisitor::visit(const PropositionalRegExp &r) {
  if (epsilon) {
    result = std::make_shared<PropositionalTrue>();
    return;
  }
  auto prop = r.get_arg();

  if (eval(*prop, this->prop_interpretation)) {
    // TODO Implement the "E(phi)" in the delta function (Brafman et. al 2018)
    result = std::make_shared<PropositionalAtom>(quote(formula.get_formula()));
  } else {
    result = std::make_shared<PropositionalTrue>();
  }
}

void DeltaBoxRegExpVisitor::visit(const TestRegExp &r) {
  NNFTransformer nnfTransformer;
  DeltaVisitor d(prop_interpretation, epsilon);
  auto regex_delta = d.apply(*nnfTransformer.apply(LDLfNot(r.get_arg())));
  auto ldlf_delta = d.apply(*formula.get_formula());
  result = std::make_shared<PropositionalOr>(
      set_prop_formulas{regex_delta, ldlf_delta});
}

void DeltaBoxRegExpVisitor::visit(const UnionRegExp &r) {
  set_prop_formulas args;
  for (const auto &regex : r.get_container()) {
    auto new_f = LDLfBox(regex, formula.get_formula());
    auto tmp = DeltaVisitor(prop_interpretation, epsilon).apply(new_f);
    args.insert(tmp);
  }
  result = std::make_shared<PropositionalAnd>(args);
}

void DeltaBoxRegExpVisitor::visit(const SequenceRegExp &) {}

void DeltaBoxRegExpVisitor::visit(const StarRegExp &) {}

std::shared_ptr<const PropositionalFormula>
DeltaBoxRegExpVisitor::apply(const RegExp &b) {
  b.accept(*this);
  return result;
}

} // namespace lydia
} // namespace whitemech