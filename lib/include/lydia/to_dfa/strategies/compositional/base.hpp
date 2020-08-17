#pragma once
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

#include <lydia/dfa/abstract_dfa.hpp>
#include <lydia/dfa/mona_dfa.hpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/strategies/bdd/delta_bdd.hpp>

namespace whitemech {
namespace lydia {

class CompositionalStrategy : public Strategy {
public:
  std::vector<atom_ptr> id2atoms;
  std::map<atom_ptr, size_t, SharedComparator> atom2ids;
  std::shared_ptr<abstract_dfa> to_dfa(const LDLfFormula &f) override;
};

class ComposeDFAVisitor : public Visitor {
public:
  CompositionalStrategy &cs;
  DFA *result;

  explicit ComposeDFAVisitor(CompositionalStrategy &cs) : cs{cs} {}

  // callbacks for LDLf
  void visit(const LDLfBooleanAtom &) override{};
  void visit(const LDLfAnd &) override{};
  void visit(const LDLfOr &) override{};
  void visit(const LDLfNot &) override{};
  void visit(const LDLfDiamond &) override{};
  void visit(const LDLfBox &) override{};

  // callbacks for regular expressions
  void visit(const PropositionalRegExp &) override{};
  void visit(const TestRegExp &) override{};
  void visit(const UnionRegExp &) override{};
  void visit(const SequenceRegExp &) override{};
  void visit(const StarRegExp &) override{};

  // callbacks for propositional logic
  void visit(const PropositionalTrue &) override{};
  void visit(const PropositionalFalse &) override{};
  void visit(const PropositionalAtom &) override{};
  void visit(const PropositionalAnd &) override{};
  void visit(const PropositionalOr &) override{};
  void visit(const PropositionalNot &) override{};

  void visit(const Symbol &) override{};
  void visit(const QuotedFormula &) override{};
  void visit(const LDLfF &) override{};
  void visit(const LDLfT &) override{};

  DFA *apply(const LDLfFormula &f) {
    f.accept(*this);
    return result;
  }
};

} // namespace lydia
} // namespace whitemech