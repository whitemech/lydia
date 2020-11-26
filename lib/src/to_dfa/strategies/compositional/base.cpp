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

DFA *ComposeDFAVisitor::apply(const LDLfFormula &f) {
  result = nullptr;
  f.accept(*this);
  return result;
}
DFA *ComposeDFARegexVisitor::apply(const RegExp &f) {
  result = nullptr;
  f.accept(*this);
  return result;
}
DFA *ComposeDFARegexVisitor::apply(const PropositionalFormula &f) {
  result = nullptr;
  f.accept(*this);
  return result;
}

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

  auto names = std::vector<std::string>();
  names.reserve(atom2ids.size());
  for (const auto &[atom, id_] : atom2ids) {
    names.push_back(atom->str());
  }

  auto visitor = ComposeDFAVisitor(*this);
  auto result = visitor.apply(*formula_nnf);
  return std::make_shared<mona_dfa>(result, names);
}

void ComposeDFAVisitor::visit(const LDLfTrue &f) { result = dfaLDLfTrue(); }
void ComposeDFAVisitor::visit(const LDLfFalse &f) { result = dfaLDLfFalse(); }

void ComposeDFAVisitor::visit(const LDLfAnd &f) {
  result = dfa_and_or<const LDLfFormula, dfaLDLfTrue, dfaAND, false>(
      f.get_container(), *this);
}

void ComposeDFAVisitor::visit(const LDLfOr &f) {
  result = dfa_and_or<const LDLfFormula, dfaLDLfFalse, dfaOR, true>(
      f.get_container(), *this);
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

  auto visitor = ComposeDFARegexVisitor(cs, current_formula_, is_diamond);
  result = visitor.apply(*f.get_regex());

  is_diamond = old_is_diamond;
  current_formula_ = old_current_formula;
}

void ComposeDFAVisitor::visit(const LDLfBox &f) {
  bool old_is_diamond = is_diamond;
  DFA *old_current_formula = current_formula_;
  is_diamond = false;
  current_formula_ = apply(*f.get_formula());

  auto visitor = ComposeDFARegexVisitor(cs, current_formula_, is_diamond);
  result = visitor.apply(*f.get_regex());

  is_diamond = old_is_diamond;
  current_formula_ = old_current_formula;
}

void ComposeDFARegexVisitor::visit(const UnionRegExp &r) {
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
    if (is_sink(final, is_diamond))
      break;
  }
  result = final;
}

void ComposeDFARegexVisitor::visit(const SequenceRegExp &r) {
  DFA *old_formula = current_formula_;
  auto subregexes = r.get_container();
  DFA *final = nullptr;
  DFA *tmp;

  for (auto it = subregexes.rbegin(); it != subregexes.rend(); it++) {
    tmp = apply(**it);
    if (final)
      dfaFree(final);
    final = dfaMinimize(tmp);
    current_formula_ = final;
    dfaFree(tmp);
  }
  result = current_formula_;
  current_formula_ = old_formula;
}

void ComposeDFARegexVisitor::visit(const StarRegExp &r) {
  DFA *body = dfaCopy(current_formula_);

  auto visitor = ComposeDFAVisitor(cs);
  DFA *regex = visitor.apply(
      *r.ctx().makeLdlfDiamond(r.get_arg(), r.ctx().makeLdlfEnd()));

  DFA *regex_or_empty = dfa_accept_empty(regex);
  DFA *star = dfa_closure(regex_or_empty, cs.indices.size(), cs.indices.data());
  if (not is_diamond) {
    dfaNegation(body);
  }
  result = dfa_concatenate(star, body, cs.indices.size(), cs.indices.data());
  if (not is_diamond) {
    dfaNegation(result);
  }
  dfaFree(regex);
  dfaFree(regex_or_empty);
  dfaFree(star);
  dfaFree(body);
}

void ComposeDFARegexVisitor::visit(const TestRegExp &r) {
  auto op = is_diamond ? dfaAND : dfaIMPL;
  DFA *tmp;
  auto visitor = ComposeDFAVisitor(cs);
  DFA *regex_dfa = visitor.apply(*r.get_arg());
  tmp = dfaProduct(regex_dfa, current_formula_, op);
  result = dfaMinimize(tmp);
  dfaFree(regex_dfa);
  dfaFree(tmp);
}

void ComposeDFARegexVisitor::visit(const PropositionalRegExp &r) {
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

void ComposeDFARegexVisitor::visit(const PropositionalTrue &f) {
  result = dfaPropositionalTrue();
}
void ComposeDFARegexVisitor::visit(const PropositionalFalse &f) {
  result = dfaLDLfFalse();
}

void ComposeDFARegexVisitor::visit(const PropositionalAtom &f) {
  int atom_index =
      cs.atom2ids[std::static_pointer_cast<const PropositionalAtom>(
          f.shared_from_this())];
  result = dfaNext(atom_index, true);
}

void ComposeDFARegexVisitor::visit(const PropositionalAnd &f) {
  result = dfa_and_or<const PropositionalFormula, dfaLDLfTrue, dfaAND, false>(
      f.get_container(), *this);
}

void ComposeDFARegexVisitor::visit(const PropositionalOr &f) {
  result = dfa_and_or<const PropositionalFormula, dfaLDLfFalse, dfaOR, true>(
      f.get_container(), *this);
}

void ComposeDFARegexVisitor::visit(const PropositionalNot &f) {
  assert(f.get_arg()->type_code_ == TypeID::t_PropositionalAtom);
  int atom_index =
      cs.atom2ids[std::static_pointer_cast<const PropositionalAtom>(
          f.get_arg())];
  result = dfaNext(atom_index, false);
}

ComposeDFARegexVisitor::~ComposeDFARegexVisitor() {
  if (current_formula_) {
    dfaFree(current_formula_);
  }
}

} // namespace whitemech::lydia