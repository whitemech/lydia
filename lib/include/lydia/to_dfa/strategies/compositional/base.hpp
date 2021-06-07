#pragma once
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

#include <lydia/dfa/abstract_dfa.hpp>
#include <lydia/dfa/mona_dfa.hpp>
#include <lydia/logic/atom_visitor.hpp>
#include <lydia/logic/ldlf/only_test.hpp>
#include <lydia/logic/nnf.hpp>
#include <lydia/mona_ext/mona_ext_base.hpp>
#include <lydia/to_dfa/core.hpp>
#include <numeric>
#include <queue>

namespace whitemech::lydia {

class CompositionalStrategy : public Strategy {
private:
  CUDD::Cudd* prop_mgr;
  void reset();

public:
  CompositionalStrategy() { prop_mgr = new CUDD::Cudd(0, 0, 0, 0, 0); }
  ~CompositionalStrategy() { delete prop_mgr; }
  set_atoms_ptr atoms;
  std::vector<atom_ptr> id2atoms;
  std::map<atom_ptr, size_t, SharedComparator> atom2ids;
  std::vector<int> indices;
  std::shared_ptr<abstract_dfa> to_dfa(const LDLfFormula& f) override;
  DFA* to_dfa_internal(const LDLfFormula& f, set_atoms_ptr atoms);

  DFA* star(const RegExp& r, DFA* body);
};

class AComposeDFAVisitor : public Visitor {
public:
  virtual DFA* apply(const LDLfFormula& f) { return nullptr; };
  virtual DFA* apply(const RegExp& f) { return nullptr; };
  virtual DFA* apply(const PropositionalFormula& f) { return nullptr; };
};

class ComposeDFAVisitor : public AComposeDFAVisitor {
private:
  DFA* current_body_dfa_ = nullptr;
  ldlf_ptr* current_body_ = nullptr;
  bool is_diamond = false;

public:
  CompositionalStrategy& cs;
  DFA* result;

  explicit ComposeDFAVisitor(CompositionalStrategy& cs) : cs{cs} {}

  // callbacks for LDLf
  void visit(const LDLfTrue& f) override;
  void visit(const LDLfFalse& f) override;
  void visit(const LDLfAnd& f) override;
  void visit(const LDLfOr&) override;
  void visit(const LDLfNot& f) override;
  void visit(const LDLfDiamond&) override;
  void visit(const LDLfBox&) override;

  void visit(const Symbol&) override{};
  void visit(const QuotedFormula&) override{};
  void visit(const LDLfF&) override{};
  void visit(const LDLfT&) override{};

  DFA* apply(const LDLfFormula& f) override;
};

class ComposeDFARegexVisitor : public AComposeDFAVisitor {
private:
  DFA* current_formula_ = nullptr;
  bool is_diamond;

  static bool is_atomic_until_test_(const RegExp& r);
  void test_free_star_(const StarRegExp&);
  void general_star_(const StarRegExp&);

public:
  CompositionalStrategy& cs;
  DFA* result;
  explicit ComposeDFARegexVisitor(CompositionalStrategy& cs,
                                  DFA* current_formula, bool is_diamond)
      : cs{cs}, current_formula_{current_formula}, is_diamond{is_diamond} {}
  ~ComposeDFARegexVisitor();

  // callbacks for regular expressions
  void visit(const PropositionalRegExp&) override;
  void visit(const TestRegExp&) override;
  void visit(const UnionRegExp&) override;
  void visit(const SequenceRegExp&) override;
  void visit(const StarRegExp&) override;

  // callbacks for propositional logic
  void visit(const PropositionalTrue&) override;
  void visit(const PropositionalFalse&) override;
  void visit(const PropositionalAtom&) override;
  void visit(const PropositionalAnd&) override;
  void visit(const PropositionalOr&) override;
  void visit(const PropositionalNot&) override;

  DFA* apply(const RegExp& f) override;
  DFA* apply(const PropositionalFormula& f) override;
};

template <typename T, DFA* (*dfaMaker)(void), dfaProductType productType,
          bool is_positive>
DFA* dfa_and_or(std::set<std::shared_ptr<T>, SharedComparator> container,
                AComposeDFAVisitor& v) {
  DFA* tmp1;
  DFA* final;

  auto dfas = std::vector<DFA*>();
  dfas.reserve(container.size());
  for (const auto& subf : container) {
    tmp1 = v.apply(*subf);
    if (is_sink(tmp1, is_positive)) {
      for (const auto dfa_to_free : dfas) {
        dfaFree(dfa_to_free);
      }
      return tmp1;
    }
    dfas.push_back(tmp1);
  }

  auto cmp = [](const DFA* d1, const DFA* d2) { return d1->ns > d2->ns; };
  std::priority_queue<DFA*, std::vector<DFA*>, decltype(cmp)> queue(
      dfas.begin(), dfas.end(), cmp);
  while (queue.size() > 1) {
    DFA* lhs = queue.top();
    queue.pop();
    DFA* rhs = queue.top();
    queue.pop();
    tmp1 = dfaProduct(lhs, rhs, productType);
    final = dfaMinimize(tmp1);
    dfaFree(lhs);
    dfaFree(rhs);
    dfaFree(tmp1);
    if (is_sink(final, is_positive)) {
      while (!queue.empty()) {
        dfaFree(queue.top());
        queue.pop();
      }
      return final;
    }
    queue.push(final);
  }
  final = queue.top();
  return final;
}

} // namespace whitemech::lydia