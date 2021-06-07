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

#include <lydia/logic/ldlf/test_free.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>
#include <lydia/to_dfa/strategies/compositional/star.hpp>
#include <utility>

namespace whitemech::lydia {

DFA* ComposeDFAVisitor::apply(const LDLfFormula& f) {
  result = nullptr;
  f.accept(*this);
  return result;
}
DFA* ComposeDFARegexVisitor::apply(const RegExp& f) {
  result = nullptr;
  f.accept(*this);
  return result;
}
DFA* ComposeDFARegexVisitor::apply(const PropositionalFormula& f) {
  result = nullptr;
  f.accept(*this);
  return result;
}

std::shared_ptr<abstract_dfa>
CompositionalStrategy::to_dfa(const LDLfFormula& formula) {
  reset();
  auto formula_nnf = to_nnf(formula);
  auto atoms_set = find_atoms(*formula_nnf);
  auto names = std::vector<std::string>();
  names.reserve(atoms_set.size());
  for (const auto& atom : atoms_set) {
    names.push_back(atom->str());
  }
  auto result = to_dfa_internal(*formula_nnf, atoms_set);
  return std::make_shared<mona_dfa>(result, names);
}

DFA* CompositionalStrategy::to_dfa_internal(const LDLfFormula& f,
                                            set_atoms_ptr atoms_set) {
  int index = 0;
  atoms = std::move(atoms_set);
  for (const auto& atom : atoms) {
    atom2ids[atom] = index;
    id2atoms.push_back(atom);
    index++;
  }
  indices = std::vector<int>(atom2ids.size());
  std::iota(indices.begin(), indices.end(), 0);
  auto visitor = ComposeDFAVisitor(*this);
  auto result = visitor.apply(f);
  return result;
}

void CompositionalStrategy::reset() {
  atoms = set_atoms_ptr{};
  id2atoms = std::vector<atom_ptr>{};
  atom2ids = std::map<atom_ptr, size_t, SharedComparator>{};
  indices = std::vector<int>{};
}

void ComposeDFAVisitor::visit(const LDLfTrue& f) { result = dfaLDLfTrue(); }
void ComposeDFAVisitor::visit(const LDLfFalse& f) { result = dfaLDLfFalse(); }

void ComposeDFAVisitor::visit(const LDLfAnd& f) {
  result = dfa_and_or<const LDLfFormula, dfaLDLfTrue, dfaAND, false>(
      f.get_container(), *this);
}

void ComposeDFAVisitor::visit(const LDLfOr& f) {
  result = dfa_and_or<const LDLfFormula, dfaLDLfFalse, dfaOR, true>(
      f.get_container(), *this);
}

void ComposeDFAVisitor::visit(const LDLfNot& f) {
  DFA* tmp = apply(*f.get_arg());
  dfaNegation(tmp);
  result = dfaMinimize(tmp);
  dfaFree(tmp);
}

void ComposeDFAVisitor::visit(const LDLfDiamond& f) {
  bool old_is_diamond = is_diamond;
  DFA* old_current_body_dfa = current_body_dfa_;
  is_diamond = true;
  current_body_dfa_ = apply(*f.get_formula());

  auto visitor = ComposeDFARegexVisitor(cs, current_body_dfa_, is_diamond);
  result = visitor.apply(*f.get_regex());

  is_diamond = old_is_diamond;
  current_body_dfa_ = old_current_body_dfa;
}

void ComposeDFAVisitor::visit(const LDLfBox& f) {
  bool old_is_diamond = is_diamond;
  DFA* old_current_body_dfa = current_body_dfa_;
  is_diamond = false;
  current_body_dfa_ = apply(*f.get_formula());

  auto visitor = ComposeDFARegexVisitor(cs, current_body_dfa_, is_diamond);
  result = visitor.apply(*f.get_regex());

  is_diamond = old_is_diamond;
  current_body_dfa_ = old_current_body_dfa;
}

void ComposeDFARegexVisitor::visit(const UnionRegExp& r) {
  DFA* tmp1;
  DFA* tmp2;
  DFA* tmp3;
  DFA* final = is_diamond ? dfaLDLfFalse() : dfaLDLfTrue();
  auto op = is_diamond ? dfaOR : dfaAND;
  for (const auto& x : r.get_container()) {
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

void ComposeDFARegexVisitor::visit(const SequenceRegExp& r) {
  DFA* old_formula = current_formula_;
  auto subregexes = r.get_container();
  DFA* final = nullptr;
  DFA* tmp;

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

void ComposeDFARegexVisitor::visit(const StarRegExp& r) {
  if (is_test_free(r) or is_atomic_until_test_(*r.get_arg())) {
    test_free_star_(r);
  } else {
    general_star_(r);
  }
}

void ComposeDFARegexVisitor::test_free_star_(const StarRegExp& r) {
  DFA* tmp;
  DFA* body = dfaCopy(current_formula_);

  auto visitor = ComposeDFAVisitor(cs);
  DFA* regex = visitor.apply(
      *r.ctx().makeLdlfDiamond(r.get_arg(), r.ctx().makeLdlfEnd()));

  DFA* regex_or_empty = dfa_accept_empty(regex);
  DFA* star = dfa_closure(regex_or_empty, cs.indices.size(), cs.indices.data());
  if (not is_diamond) {
    dfaNegation(body);
  }
  tmp = dfa_concatenate(star, body, cs.indices.size(), cs.indices.data());
  if (not is_diamond) {
    dfaNegation(tmp);
  }
  result = dfaMinimize(tmp);
  dfaFree(tmp);
  dfaFree(regex);
  dfaFree(regex_or_empty);
  dfaFree(star);
  dfaFree(body);
}

void ComposeDFARegexVisitor::general_star_(const StarRegExp& r) {
  DFA* body = dfaCopy(current_formula_);
  if (not is_diamond) {
    dfaNegation(body);
  }
  DFA* tmp = cs.star(r, body);
  if (not is_diamond) {
    dfaNegation(tmp);
  }
  result = dfaMinimize(tmp);
  dfaFree(tmp);
  dfaFree(body);
}

void ComposeDFARegexVisitor::visit(const TestRegExp& r) {
  auto op = is_diamond ? dfaAND : dfaIMPL;
  DFA* tmp;
  auto visitor = ComposeDFAVisitor(cs);
  DFA* regex_dfa = visitor.apply(*r.get_arg());
  tmp = dfaProduct(regex_dfa, current_formula_, op);
  result = dfaMinimize(tmp);
  dfaFree(regex_dfa);
  dfaFree(tmp);
}

void ComposeDFARegexVisitor::visit(const PropositionalRegExp& r) {
  DFA* regex = apply(*r.get_arg());
  DFA* body = current_formula_;

  if (is_diamond) {
    result =
        dfaLDLfDiamondProp(regex, body, cs.indices.size(), cs.indices.data());
  } else {
    DFA* tmp = dfaCopy(body);
    dfaNegation(tmp);
    result =
        dfaLDLfDiamondProp(regex, tmp, cs.indices.size(), cs.indices.data());
    dfaNegation(result);
    dfaFree(tmp);
  }
  dfaFree(regex);
}

void ComposeDFARegexVisitor::visit(const PropositionalTrue& f) {
  result = dfaPropositionalTrue();
}
void ComposeDFARegexVisitor::visit(const PropositionalFalse& f) {
  result = dfaLDLfFalse();
}

void ComposeDFARegexVisitor::visit(const PropositionalAtom& f) {
  int atom_index =
      cs.atom2ids[std::static_pointer_cast<const PropositionalAtom>(
          f.shared_from_this())];
  result = dfaNext(atom_index, true);
}

void ComposeDFARegexVisitor::visit(const PropositionalAnd& f) {
  result = dfa_and_or<const PropositionalFormula, dfaLDLfTrue, dfaAND, false>(
      f.get_container(), *this);
}

void ComposeDFARegexVisitor::visit(const PropositionalOr& f) {
  result = dfa_and_or<const PropositionalFormula, dfaLDLfFalse, dfaOR, true>(
      f.get_container(), *this);
}

void ComposeDFARegexVisitor::visit(const PropositionalNot& f) {
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

bool ComposeDFARegexVisitor::is_atomic_until_test_(const RegExp& r) {
  if (!is_a<SequenceRegExp>(r))
    return false;

  auto seq_regex = dynamic_cast<const SequenceRegExp&>(r);
  if (seq_regex.get_container().size() != 2)
    return false;

  auto first_regex = *seq_regex.get_container().begin();
  auto second_regex = *(seq_regex.get_container().begin() + 1);

  if (!is_a<TestRegExp>(*first_regex) ||
      !is_a<PropositionalRegExp>(*second_regex))
    return false;

  auto test_regex = dynamic_cast<const TestRegExp&>(*first_regex);
  auto prop_regex = dynamic_cast<const PropositionalRegExp&>(*second_regex);

  if (!is_a<LDLfDiamond>(*test_regex.get_arg()) ||
      !is_a<PropositionalTrue>(*prop_regex.get_arg())) {
    return false;
  }

  auto diamond_formula =
      dynamic_cast<const LDLfDiamond&>(*test_regex.get_arg());
  auto regex = diamond_formula.get_regex();
  auto formula = diamond_formula.get_formula();
  return is_a<PropositionalRegExp>(*regex) and is_a<LDLfTrue>(*formula);
}

} // namespace whitemech::lydia