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

#include <cassert>
#include <lydia/logger.hpp>
#include <lydia/logic/nnf.hpp>
#include <lydia/logic/pl/base.hpp>
#include <lydia/to_dfa/delta_symbolic.hpp>

namespace whitemech::lydia {

Logger DeltaSymbolicVisitor::logger = Logger("delta");

void DeltaSymbolicVisitor::visit(const LDLfTrue& x) {
  result = x.ctx().makeTrue();
}
void DeltaSymbolicVisitor::visit(const LDLfFalse& x) {
  result = x.ctx().makeFalse();
}

void DeltaSymbolicVisitor::visit(const LDLfNot& b) {
  DeltaSymbolicVisitor::logger.error(
      "Delta function should not be called with a not.");
  assert(false);
}

void DeltaSymbolicVisitor::visit(const LDLfAnd& x) {
  auto container = x.get_container();
  set_prop_formulas new_container;
  for (auto& a : container) {
    new_container.insert(apply(*a));
  }
  result = x.ctx().makePropAnd(new_container);
}

void DeltaSymbolicVisitor::visit(const LDLfOr& x) {
  auto container = x.get_container();
  set_prop_formulas new_container;
  for (auto& a : container) {
    new_container.insert(apply(*a));
  }
  result = x.ctx().makePropOr(new_container);
}

void DeltaSymbolicVisitor::visit(const LDLfDiamond& f) {
  auto d = DeltaSymbolicDiamondRegExpVisitor(f, epsilon);
  result = d.apply(*f.get_regex());
}

void DeltaSymbolicVisitor::visit(const LDLfBox& f) {
  auto d = DeltaSymbolicBoxRegExpVisitor(f, epsilon);
  result = d.apply(*f.get_regex());
}

std::shared_ptr<const PropositionalFormula>
DeltaSymbolicVisitor::apply(const LDLfFormula& b) {
  b.accept(*this);
  return result;
}

std::shared_ptr<const PropositionalFormula> delta_symbolic(const LDLfFormula& x,
                                                           bool epsilon) {
  DeltaSymbolicVisitor deltaSymbolicVisitor{epsilon};
  return deltaSymbolicVisitor.apply(x);
}

void DeltaSymbolicDiamondRegExpVisitor::visit(const PropositionalRegExp& r) {
  if (epsilon) {
    result = r.ctx().makeFalse();
    return;
  }
  auto prop = r.get_arg();

  ExpandVisitor e;
  auto expanded_formula = e.apply(*formula.get_formula());
  auto quoted_formula = r.ctx().makePropAtom(quote(expanded_formula));
  result = r.ctx().makePropAnd({quoted_formula, prop});
}

void DeltaSymbolicDiamondRegExpVisitor::visit(const TestRegExp& r) {
  auto d = DeltaSymbolicVisitor(epsilon);
  auto regex_delta = d.apply(*r.get_arg());
  auto ldlf_delta = d.apply(*formula.get_formula());
  result = r.ctx().makePropAnd(set_prop_formulas{regex_delta, ldlf_delta});
}

void DeltaSymbolicDiamondRegExpVisitor::visit(const UnionRegExp& r) {
  set_prop_formulas args;
  for (const auto& regex : r.get_container()) {
    auto new_f = r.ctx().makeLdlfDiamond(regex, formula.get_formula());
    auto tmp = DeltaSymbolicVisitor(epsilon).apply(*new_f);
    args.insert(tmp);
  }
  result = r.ctx().makePropOr(args);
}

void DeltaSymbolicDiamondRegExpVisitor::visit(const SequenceRegExp& r) {
  const regex_ptr head = r.get_container().front();
  regex_ptr tail;
  if (r.get_container().size() == 2) {
    tail = r.get_container().back();
  } else {
    tail = r.ctx().makeSeqRegex(
        vec_regex(r.get_container().begin() + 1, r.get_container().end()));
  }
  ldlf_ptr ldlf_tail = r.ctx().makeLdlfDiamond(tail, formula.get_formula());
  result = DeltaSymbolicVisitor(epsilon).apply(
      *r.ctx().makeLdlfDiamond(head, ldlf_tail));
}

void DeltaSymbolicDiamondRegExpVisitor::visit(const StarRegExp& r) {
  auto d = DeltaSymbolicVisitor(epsilon);
  auto phi = d.apply(*formula.get_formula());
  auto next =
      r.ctx().makeLdlfDiamond(formula.get_regex(), formula.get_formula());
  auto auxiliary_false = r.ctx().makeLdlfF(next);
  auto phi2 = d.apply(*r.ctx().makeLdlfDiamond(r.get_arg(), auxiliary_false));
  result = r.ctx().makePropOr(set_prop_formulas{phi, phi2});
}

std::shared_ptr<const PropositionalFormula>
DeltaSymbolicDiamondRegExpVisitor::apply(const RegExp& b) {
  b.accept(*this);
  return result;
}

void DeltaSymbolicBoxRegExpVisitor::visit(const PropositionalRegExp& r) {
  if (epsilon) {
    result = r.ctx().makeTrue();
    return;
  }
  auto prop = r.get_arg();

  ExpandVisitor e;
  auto expanded_formula = e.apply(*formula.get_formula());
  auto quoted_formula = r.ctx().makePropAtom(quote(expanded_formula));
  result = r.ctx().makePropOr({quoted_formula, prop->logical_not()});
}

void DeltaSymbolicBoxRegExpVisitor::visit(const TestRegExp& r) {
  NNFTransformer nnfTransformer;
  DeltaSymbolicVisitor d(epsilon);
  auto regex_delta =
      d.apply(*nnfTransformer.apply(LDLfNot(r.ctx(), r.get_arg())));
  auto ldlf_delta = d.apply(*formula.get_formula());
  result = r.ctx().makePropOr(set_prop_formulas{regex_delta, ldlf_delta});
}

void DeltaSymbolicBoxRegExpVisitor::visit(const UnionRegExp& r) {
  set_prop_formulas args;
  for (const auto& regex : r.get_container()) {
    auto new_f = r.ctx().makeLdlfBox(regex, formula.get_formula());
    auto tmp = DeltaSymbolicVisitor(epsilon).apply(*new_f);
    args.insert(tmp);
  }
  result = r.ctx().makePropAnd(args);
}

void DeltaSymbolicBoxRegExpVisitor::visit(const SequenceRegExp& r) {
  const regex_ptr head = r.get_container().front();
  regex_ptr tail;
  if (r.get_container().size() == 2) {
    tail = r.get_container().back();
  } else {
    tail = r.ctx().makeSeqRegex(
        vec_regex(r.get_container().begin() + 1, r.get_container().end()));
  }
  ldlf_ptr ldlf_tail = r.ctx().makeLdlfBox(tail, formula.get_formula());
  result = DeltaSymbolicVisitor(epsilon).apply(
      *r.ctx().makeLdlfBox(head, ldlf_tail));
}

void DeltaSymbolicBoxRegExpVisitor::visit(const StarRegExp& r) {
  auto d = DeltaSymbolicVisitor(epsilon);
  auto phi = d.apply(*formula.get_formula());
  auto next = r.ctx().makeLdlfBox(formula.get_regex(), formula.get_formula());
  auto auxiliary_true = r.ctx().makeLdlfT(next);
  auto phi2 = d.apply(*r.ctx().makeLdlfBox(r.get_arg(), auxiliary_true));
  result = r.ctx().makePropAnd(set_prop_formulas{phi, phi2});
}

void DeltaSymbolicVisitor::visit(const LDLfF& f) {
  result = f.ctx().makeFalse();
}

void DeltaSymbolicVisitor::visit(const LDLfT& f) {
  result = f.ctx().makeTrue();
}

std::shared_ptr<const PropositionalFormula>
DeltaSymbolicBoxRegExpVisitor::apply(const RegExp& b) {
  b.accept(*this);
  return result;
}

} // namespace whitemech::lydia