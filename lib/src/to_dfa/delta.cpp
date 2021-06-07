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
#include <lydia/ast/base.hpp>
#include <lydia/logger.hpp>
#include <lydia/logic/nnf.hpp>
#include <lydia/logic/pl/base.hpp>
#include <lydia/logic/pl/eval.hpp>
#include <lydia/to_dfa/delta.hpp>

namespace whitemech::lydia {

Logger DeltaVisitor::logger = Logger("delta");

void DeltaVisitor::visit(const LDLfTrue& x) { result = x.ctx().makeTrue(); }
void DeltaVisitor::visit(const LDLfFalse& x) { result = x.ctx().makeFalse(); }

void DeltaVisitor::visit(const LDLfNot& b) {
  DeltaVisitor::logger.error("Delta function should not be called with a not.");
  assert(false);
}

void DeltaVisitor::visit(const LDLfAnd& x) {
  auto container = x.get_container();
  set_prop_formulas new_container;
  for (auto& a : container) {
    new_container.insert(apply(*a));
  }
  result = x.ctx().makePropAnd(new_container);
}

void DeltaVisitor::visit(const LDLfOr& x) {
  auto container = x.get_container();
  set_prop_formulas new_container;
  for (auto& a : container) {
    new_container.insert(apply(*a));
  }
  result = x.ctx().makePropOr(new_container);
}

void DeltaVisitor::visit(const LDLfDiamond& f) {
  auto d = DeltaDiamondRegExpVisitor(prop_interpretation, f, epsilon);
  result = d.apply(*f.get_regex());
}

void DeltaVisitor::visit(const LDLfBox& f) {
  auto d = DeltaBoxRegExpVisitor(prop_interpretation, f, epsilon);
  result = d.apply(*f.get_regex());
}

std::shared_ptr<const PropositionalFormula>
DeltaVisitor::apply(const LDLfFormula& b) {
  b.accept(*this);
  return result;
}

std::shared_ptr<const PropositionalFormula> delta(const LDLfFormula& x) {
  // epsilon = true
  DeltaVisitor deltaVisitor;
  return deltaVisitor.apply(x);
}

std::shared_ptr<const PropositionalFormula> delta(const LDLfFormula& x,
                                                  const set_atoms_ptr& i) {
  DeltaVisitor deltaVisitor{i};
  return deltaVisitor.apply(x);
}

void DeltaDiamondRegExpVisitor::visit(const PropositionalRegExp& r) {
  if (epsilon) {
    result = r.ctx().makeFalse();
    return;
  }
  auto prop = r.get_arg();

  if (eval(*prop, this->prop_interpretation)) {
    ExpandVisitor e;
    result = r.ctx().makePropAtom(quote(e.apply(*formula.get_formula())));
  } else {
    result = r.ctx().makeFalse();
  }
}

void DeltaDiamondRegExpVisitor::visit(const TestRegExp& r) {
  auto d = DeltaVisitor(prop_interpretation, epsilon);
  auto regex_delta = d.apply(*r.get_arg());
  auto ldlf_delta = d.apply(*formula.get_formula());
  result = r.ctx().makePropAnd(set_prop_formulas{regex_delta, ldlf_delta});
}

void DeltaDiamondRegExpVisitor::visit(const UnionRegExp& r) {
  set_prop_formulas args;
  for (const auto& regex : r.get_container()) {
    auto new_f = r.ctx().makeLdlfDiamond(regex, formula.get_formula());
    auto tmp = DeltaVisitor(prop_interpretation, epsilon).apply(*new_f);
    args.insert(tmp);
  }
  result = r.ctx().makePropOr(args);
}

void DeltaDiamondRegExpVisitor::visit(const SequenceRegExp& r) {
  const regex_ptr head = r.get_container().front();
  regex_ptr tail;
  if (r.get_container().size() == 2) {
    tail = r.get_container().back();
  } else {
    tail = r.ctx().makeSeqRegex(
        vec_regex(r.get_container().begin() + 1, r.get_container().end()));
  }
  ldlf_ptr ldlf_tail = r.ctx().makeLdlfDiamond(tail, formula.get_formula());
  result = DeltaVisitor(prop_interpretation, epsilon)
               .apply(*r.ctx().makeLdlfDiamond(head, ldlf_tail));
}

void DeltaDiamondRegExpVisitor::visit(const StarRegExp& r) {
  auto d = DeltaVisitor(prop_interpretation, epsilon);
  auto phi = d.apply(*formula.get_formula());
  auto f = r.ctx().makeLdlfF(
      std::static_pointer_cast<const LDLfFormula>(formula.shared_from_this()));
  auto phi2 = d.apply(*r.ctx().makeLdlfDiamond(r.get_arg(), f));
  result = r.ctx().makePropOr(set_prop_formulas{phi, phi2});
}

std::shared_ptr<const PropositionalFormula>
DeltaDiamondRegExpVisitor::apply(const RegExp& b) {
  b.accept(*this);
  return result;
}

void DeltaBoxRegExpVisitor::visit(const PropositionalRegExp& r) {
  if (epsilon) {
    result = r.ctx().makeTrue();
    return;
  }
  auto prop = r.get_arg();

  if (eval(*prop, this->prop_interpretation)) {
    ExpandVisitor e;
    result = r.ctx().makePropAtom(quote(e.apply(*formula.get_formula())));
  } else {
    result = r.ctx().makeTrue();
  }
}

void DeltaBoxRegExpVisitor::visit(const TestRegExp& r) {
  NNFTransformer nnfTransformer;
  DeltaVisitor d(prop_interpretation, epsilon);
  auto regex_delta =
      d.apply(*nnfTransformer.apply(LDLfNot(r.ctx(), r.get_arg())));
  auto ldlf_delta = d.apply(*formula.get_formula());
  result = r.ctx().makePropOr(set_prop_formulas{regex_delta, ldlf_delta});
}

void DeltaBoxRegExpVisitor::visit(const UnionRegExp& r) {
  set_prop_formulas args;
  for (const auto& regex : r.get_container()) {
    auto new_f = r.ctx().makeLdlfBox(regex, formula.get_formula());
    auto tmp = DeltaVisitor(prop_interpretation, epsilon).apply(*new_f);
    args.insert(tmp);
  }
  result = r.ctx().makePropAnd(args);
}

void DeltaBoxRegExpVisitor::visit(const SequenceRegExp& r) {
  const regex_ptr head = r.get_container().front();
  regex_ptr tail;
  if (r.get_container().size() == 2) {
    tail = r.get_container().back();
  } else {
    tail = r.ctx().makeSeqRegex(
        vec_regex(r.get_container().begin() + 1, r.get_container().end()));
  }
  ldlf_ptr ldlf_tail = r.ctx().makeLdlfBox(tail, formula.get_formula());
  result = DeltaVisitor(prop_interpretation, epsilon)
               .apply(*r.ctx().makeLdlfBox(head, ldlf_tail));
}

void DeltaBoxRegExpVisitor::visit(const StarRegExp& r) {
  auto d = DeltaVisitor(prop_interpretation, epsilon);
  auto phi = d.apply(*formula.get_formula());
  auto f = r.ctx().makeLdlfT(
      std::static_pointer_cast<const LDLfFormula>(formula.shared_from_this()));
  auto phi2 = d.apply(*r.ctx().makeLdlfBox(r.get_arg(), f));
  result = r.ctx().makePropAnd(set_prop_formulas{phi, phi2});
}

void DeltaVisitor::visit(const LDLfF& f) { result = f.ctx().makeFalse(); }

void DeltaVisitor::visit(const LDLfT& f) { result = f.ctx().makeTrue(); }

void DeltaVisitor::visit(const LDLfQ& f) { result = apply(*f.get_arg()); }

std::shared_ptr<const PropositionalFormula>
DeltaBoxRegExpVisitor::apply(const RegExp& b) {
  b.accept(*this);
  return result;
}

void ExpandVisitor::visit(const LDLfTrue& f) {
  result = f.ctx().makeLdlfTrue();
}
void ExpandVisitor::visit(const LDLfFalse& f) {
  result = f.ctx().makeLdlfFalse();
}

void ExpandVisitor::visit(const LDLfF& x) { result = apply(*x.get_arg()); }
void ExpandVisitor::visit(const LDLfT& x) { result = apply(*x.get_arg()); }
void ExpandVisitor::visit(const LDLfQ& x) {
  result = std::static_pointer_cast<const LDLfQ>(x.shared_from_this());
}

void ExpandVisitor::visit(const LDLfAnd& f) {
  set_formulas new_container;
  for (const auto& x : f.get_container()) {
    new_container.insert(apply(*x));
  }
  result = f.ctx().makeLdlfAnd(new_container);
}

void ExpandVisitor::visit(const LDLfOr& f) {
  set_formulas new_container;
  for (const auto& x : f.get_container()) {
    new_container.insert(apply(*x));
  }
  result = f.ctx().makeLdlfOr(new_container);
}

void ExpandVisitor::visit(const LDLfNot& f) {
  result = f.ctx().makeLdlfNot(apply(*f.get_arg()));
}

void ExpandVisitor::visit(const LDLfDiamond& f) {
  result =
      f.ctx().makeLdlfDiamond(apply(*f.get_regex()), apply(*f.get_formula()));
}
void ExpandVisitor::visit(const LDLfBox& f) {
  result = f.ctx().makeLdlfBox(apply(*f.get_regex()), apply(*f.get_formula()));
}

void ExpandVisitor::visit(const PropositionalRegExp& r) {
  regex_result = r.ctx().makePropRegex(r.get_arg());
}

void ExpandVisitor::visit(const TestRegExp& r) {
  regex_result = r.ctx().makeTestRegex(apply(*r.get_arg()));
}

void ExpandVisitor::visit(const UnionRegExp& r) {
  set_regex new_container;
  for (const auto& x : r.get_container()) {
    new_container.insert(apply(*x));
  }
  regex_result = r.ctx().makeUnionRegex(new_container);
}

void ExpandVisitor::visit(const SequenceRegExp& r) {
  vec_regex new_container;
  for (const auto& x : r.get_container()) {
    new_container.push_back(apply(*x));
  }
  regex_result = r.ctx().makeSeqRegex(new_container);
}

void ExpandVisitor::visit(const StarRegExp& r) {
  regex_result = r.ctx().makeStarRegex(apply(*r.get_arg()));
}

std::shared_ptr<const LDLfFormula> ExpandVisitor::apply(const LDLfFormula& f) {
  f.accept(*this);
  return result;
}

std::shared_ptr<const RegExp> ExpandVisitor::apply(const RegExp& f) {
  f.accept(*this);
  return regex_result;
}

} // namespace whitemech::lydia