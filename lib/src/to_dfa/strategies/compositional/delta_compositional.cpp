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
#include <iostream>
#include <lydia/logger.hpp>
#include <lydia/logic/nnf.hpp>
#include <lydia/logic/pl/base.hpp>
#include <lydia/to_dfa/delta.hpp>
#include <lydia/to_dfa/strategies/compositional/delta_compositional.hpp>

namespace whitemech::lydia {

Logger DeltaCompositionalVisitor::logger = Logger("delta");

void DeltaCompositionalVisitor::visit(const LDLfTrue& x) {
  result = mgr.bddOne();
}
void DeltaCompositionalVisitor::visit(const LDLfFalse& x) {
  result = mgr.bddZero();
}

void DeltaCompositionalVisitor::visit(const LDLfNot& b) {
  DeltaCompositionalVisitor::logger.error(
      "Delta function should not be called with a not.");
  assert(false);
}

void DeltaCompositionalVisitor::visit(const LDLfAnd& x) {
  auto container = x.get_container();
  set_prop_formulas new_container;
  CUDD::BDD tmp = mgr.bddOne();
  for (auto& a : container) {
    tmp *= apply(*a);
  }
  result = tmp;
}

void DeltaCompositionalVisitor::visit(const LDLfOr& x) {
  auto container = x.get_container();
  set_prop_formulas new_container;
  CUDD::BDD tmp = mgr.bddZero();
  for (auto& a : container) {
    tmp += apply(*a);
  }
  result = tmp;
}

void DeltaCompositionalVisitor::visit(const LDLfDiamond& f) {
  auto old_formula = formula;
  formula = f.get_formula();
  result = apply(*f.get_regex());
  formula = old_formula;
}

void DeltaCompositionalVisitor::visit(const LDLfBox& f) {
  DeltaCompositionalVisitor::logger.error(
      "Delta function should not be called with a box formula.");
  assert(false);
}

void DeltaCompositionalVisitor::add_variable_if_not_present_(
    const basic_ptr& f) {
  if (subformula2id.find(f) == subformula2id.end()) {
    size_t id = subformula2id.size();
    CUDD::BDD var = mgr.bddVar(id);
    subformula2id[f] = id;
    id2subformula.push_back(f);
    subformula_bddvars.push_back(var);
  }
}

CUDD::BDD DeltaCompositionalVisitor::get_var_(const basic_ptr& f) {
  size_t id = subformula2id[f];
  CUDD::BDD var = subformula_bddvars[id];
  return var;
}

void DeltaCompositionalVisitor::visit(const PropositionalRegExp& r) {
  auto prop = r.get_arg();
  auto regex = std::static_pointer_cast<const RegExp>(r.shared_from_this());

  ExpandVisitor e;
  auto expanded_formula = e.apply(*formula);
  auto atom = r.ctx().makeLdlfDiamond(regex, expanded_formula);
  //  auto atom = r.ctx().makeLdlfDiamond(regex, formula);
  CUDD::BDD var;
  add_variable_if_not_present_(atom);
  var = get_var_(atom);
  result = var;
}

void DeltaCompositionalVisitor::visit(const TestRegExp& r) {
  auto regex = r.shared_from_this();
  add_variable_if_not_present_(regex);
  CUDD::BDD test_var = get_var_(regex);
  CUDD::BDD body_var = apply(*formula);
  result = test_var & body_var;
}

void DeltaCompositionalVisitor::visit(const UnionRegExp& r) {
  set_prop_formulas args;
  CUDD::BDD tmp = mgr.bddZero();
  for (const auto& regex : r.get_container()) {
    auto new_f = r.ctx().makeLdlfDiamond(regex, formula);
    tmp += apply(*new_f);
  }
  result = tmp;
}

void DeltaCompositionalVisitor::visit(const SequenceRegExp& r) {
  const regex_ptr head = r.get_container().front();
  regex_ptr tail;
  if (r.get_container().size() == 2) {
    tail = r.get_container().back();
  } else {
    tail = r.ctx().makeSeqRegex(
        vec_regex(r.get_container().begin() + 1, r.get_container().end()));
  }
  ldlf_ptr ldlf_tail = r.ctx().makeLdlfDiamond(tail, formula);
  result = apply(*r.ctx().makeLdlfDiamond(head, ldlf_tail));
}

void DeltaCompositionalVisitor::visit(const StarRegExp& r) {
  auto regex = std::static_pointer_cast<const RegExp>(r.shared_from_this());
  auto phi = apply(*formula);
  auto next = r.ctx().makeLdlfDiamond(regex, formula);
  auto auxiliary_false = r.ctx().makeLdlfF(next);
  auto phi2 = apply(*r.ctx().makeLdlfDiamond(r.get_arg(), auxiliary_false));
  result = phi | phi2;
}

void DeltaCompositionalVisitor::visit(const LDLfF& f) {
  result = mgr.bddZero();
}
void DeltaCompositionalVisitor::visit(const LDLfT& f) { result = mgr.bddOne(); }
void DeltaCompositionalVisitor::visit(const LDLfQ& f) {
  auto atom = f.shared_from_this();
  add_variable_if_not_present_(atom);
  result = get_var_(atom);
}

CUDD::BDD DeltaCompositionalVisitor::apply(const RegExp& r) {
  r.accept(*this);
  return result;
}

CUDD::BDD DeltaCompositionalVisitor::apply(const LDLfFormula& f) {
  auto old_formula = formula;
  f.accept(*this);
  formula = old_formula;
  return result;
}

} // namespace whitemech::lydia