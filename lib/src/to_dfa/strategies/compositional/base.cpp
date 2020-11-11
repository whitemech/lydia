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

#include <lydia/to_dfa/strategies/compositional/base.hpp>

namespace whitemech::lydia {

std::shared_ptr<abstract_dfa>
CompositionalStrategy::to_dfa(const LDLfFormula &formula) {
  auto formula_nnf = to_nnf(formula);
  set_atoms_ptr atoms = find_atoms(*formula_nnf);
  int index = 0;
  for (const auto &atom : atoms) {
    atom2ids[atom] = index;
    id2atoms.push_back(atom);
    index++;
  }
  indices = std::vector<int>(atom2ids.size());
  std::iota(indices.begin(), indices.end(), 0);

  auto visitor = ComposeDFAVisitor(*this);
  auto result = visitor.apply(*formula_nnf);
  return std::make_shared<mona_dfa>(result, index);
}

void ComposeDFAVisitor::visit(const LDLfTrue &f) { result = dfaLDLfTrue(); }
void ComposeDFAVisitor::visit(const LDLfFalse &f) { result = dfaLDLfFalse(); }

void ComposeDFAVisitor::visit(const LDLfAnd &f) {
  DFA *tmp1;
  DFA *tmp2;
  DFA *final = dfaLDLfTrue();
  for (const auto &subf : f.get_container()) {
    tmp1 = final;
    tmp2 = apply(*subf);
    final = dfaProduct(tmp1, tmp2, dfaAND);
    final = dfaMinimize(final);
    dfaFree(tmp1);
    dfaFree(tmp2);
  }
  result = final;
}

void ComposeDFAVisitor::visit(const LDLfOr &f) {
  DFA *tmp1 = dfaLDLfFalse();
  DFA *tmp2;
  DFA *final = tmp1;
  for (const auto &subf : f.get_container()) {
    tmp1 = final;
    tmp2 = apply(*subf);
    final = dfaProduct(tmp1, tmp2, dfaOR);
    final = dfaMinimize(final);
    dfaFree(tmp1);
    dfaFree(tmp2);
  }
  result = final;
}

void ComposeDFAVisitor::visit(const LDLfNot &f) {
  DFA *tmp = apply(*f.get_arg());
  dfaNegation(tmp);
  result = dfaMinimize(tmp);
  dfaFree(tmp);
}

void ComposeDFAVisitor::visit(const LDLfDiamond &f) {
  bool old_is_diamond = is_diamond;
  DFA *old_current_formula = current_formula_;
  is_diamond = true;
  current_formula_ = apply(*f.get_formula());

  result = apply(*f.get_regex());

  is_diamond = old_is_diamond;
  current_formula_ = old_current_formula;
}

void ComposeDFAVisitor::visit(const LDLfBox &f) {
  bool old_is_diamond = is_diamond;
  DFA *old_current_formula = current_formula_;
  is_diamond = false;
  current_formula_ = apply(*f.get_formula());

  result = apply(*f.get_regex());

  is_diamond = old_is_diamond;
  current_formula_ = old_current_formula;
}

void ComposeDFAVisitor::visit(const UnionRegExp &r) {
  DFA *tmp1;
  DFA *tmp2;
  DFA *tmp3;
  DFA *final = is_diamond ? dfaLDLfFalse() : dfaLDLfTrue();
  auto op = is_diamond ? dfaOR : dfaAND;
  for (const auto &x : r.get_container()) {
    tmp1 = final;
    tmp2 = apply(*x);
    tmp3 = dfaProduct(tmp1, tmp2, op);
    final = dfaMinimize(tmp3);
    dfaFree(tmp1);
    dfaFree(tmp2);
    dfaFree(tmp3);
  }
  result = final;
}

void ComposeDFAVisitor::visit(const SequenceRegExp &r) {
  DFA *old_formula = current_formula_;
  auto subregexes = r.get_container();
  DFA *final;
  DFA *tmp;

  for (auto it = subregexes.rbegin(); it != subregexes.rend(); it++) {
    tmp = apply(**it);
    final = dfaMinimize(tmp);
    current_formula_ = final;
    dfaFree(tmp);
  }
  result = final;
  current_formula_ = old_formula;
}

void ComposeDFAVisitor::visit(const StarRegExp &r) {
  DFA *regex =
      apply(*r.ctx().makeLdlfDiamond(r.get_arg(), r.ctx().makeLdlfEnd()));
  dfa_accept_empty(regex);
  DFA *star = dfa_closure(regex, cs.indices.size(), cs.indices.data());
  if (not is_diamond) {
    dfaNegation(current_formula_);
  }
  result = dfa_concatenate(star, current_formula_, cs.indices.size(),
                           cs.indices.data());
  if (not is_diamond) {
    dfaNegation(result);
  }
  dfaFree(regex);
  dfaFree(star);
  dfaFree(current_formula_);
}

void ComposeDFAVisitor::visit(const TestRegExp &r) {
  auto op = is_diamond ? dfaAND : dfaIMPL;
  DFA *tmp;
  DFA *regex_dfa = apply(*r.get_arg());
  tmp = dfaProduct(regex_dfa, current_formula_, op);
  result = dfaMinimize(tmp);
  dfaFree(regex_dfa);
  dfaFree(tmp);
  dfaFree(current_formula_);
}

void ComposeDFAVisitor::visit(const PropositionalRegExp &r) {
  DFA *regex = apply(*r.get_arg());
  DFA *body = current_formula_;

  if (is_diamond) {
    result =
        dfaLDLfDiamondProp(regex, body, cs.indices.size(), cs.indices.data());
  } else {
    DFA *tmp = dfaCopy(body);
    dfaNegation(tmp);
    result =
        dfaLDLfDiamondProp(regex, tmp, cs.indices.size(), cs.indices.data());
    dfaNegation(result);
    dfaFree(tmp);
  }
  dfaFree(regex);
}

void ComposeDFAVisitor::visit(const PropositionalTrue &f) {
  result = dfaPropositionalTrue();
}
void ComposeDFAVisitor::visit(const PropositionalFalse &f) {
  result = dfaLDLfFalse();
}

void ComposeDFAVisitor::visit(const PropositionalAtom &f) {
  int atom_index =
      cs.atom2ids[std::static_pointer_cast<const PropositionalAtom>(
          f.shared_from_this())];
  result = dfaNext(atom_index, true);
}

void ComposeDFAVisitor::visit(const PropositionalAnd &f) {
  DFA *tmp1 = dfaLDLfTrue();
  DFA *tmp2;
  DFA *tmp3;
  DFA *dfa_and = tmp1;
  for (const auto &subf : f.get_container()) {
    tmp1 = dfa_and;
    tmp2 = apply(*subf);
    tmp3 = dfaProduct(tmp1, tmp2, dfaAND);
    dfa_and = dfaMinimize(tmp3);
    dfaFree(tmp1);
    dfaFree(tmp2);
    dfaFree(tmp3);
  }
  result = dfa_and;
}

void ComposeDFAVisitor::visit(const PropositionalOr &f) {
  DFA *tmp1 = dfaLDLfFalse();
  DFA *tmp2;
  DFA *tmp3;
  DFA *dfa_and = tmp1;
  for (const auto &subf : f.get_container()) {
    tmp1 = dfa_and;
    tmp2 = apply(*subf);
    tmp3 = dfaProduct(tmp1, tmp2, dfaOR);
    dfa_and = dfaMinimize(tmp3);
    dfaFree(tmp1);
    dfaFree(tmp2);
    dfaFree(tmp3);
  }
  result = dfa_and;
}

void ComposeDFAVisitor::visit(const PropositionalNot &f) {
  assert(f.get_arg()->type_code_ == TypeID::t_PropositionalAtom);
  int atom_index =
      cs.atom2ids[std::static_pointer_cast<const PropositionalAtom>(
          f.get_arg())];
  result = dfaNext(atom_index, false);
}

} // namespace whitemech::lydia