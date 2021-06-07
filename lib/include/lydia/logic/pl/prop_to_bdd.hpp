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

#include <lydia/logic/ldlf/base.hpp>
#include <lydia/visitor.hpp>
#include <utility>

namespace whitemech::lydia {

class PropToBDDVisitor : public Visitor {
private:
protected:
  CUDD::Cudd* mgr;
  bool initialized = false;
  std::map<atom_ptr, size_t, SharedComparator> atom2id;
  std::vector<atom_ptr> id2atom;

public:
  CUDD::BDD result;

  explicit PropToBDDVisitor(CUDD::Cudd& mgr,
                            const std::vector<atom_ptr>& id2atom_)
      : mgr{&mgr}, initialized{false} {
    id2atom = id2atom_;
    for (size_t i = 0; i < id2atom_.size(); ++i) {
      atom2id[id2atom[i]] = i;
      mgr.bddVar(i);
    }
  };
  explicit PropToBDDVisitor(const std::vector<atom_ptr>& id2atom_) {
    mgr = new CUDD::Cudd();
    initialized = true;
    id2atom = id2atom_;
    for (size_t i = 0; i < id2atom_.size(); ++i) {
      atom2id[id2atom[i]] = i;
    }
  };
  ~PropToBDDVisitor() {
    if (initialized) {
      delete mgr;
    }
  }

  // callbacks for propositional logic
  void visit(const PropositionalTrue&) override;
  void visit(const PropositionalFalse&) override;
  void visit(const PropositionalAtom&) override;
  void visit(const PropositionalAnd&) override;
  void visit(const PropositionalOr&) override;
  void visit(const PropositionalNot&) override;

  CUDD::BDD apply(const PropositionalFormula& f);
};

} // namespace whitemech::lydia