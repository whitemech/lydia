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

#include <iostream>
#include <lydia/logic/atom_visitor.hpp>
#include <lydia/logic/pl/cnf.hpp>
#include <lydia/logic/pl/eval.hpp>
#include <lydia/logic/pl/models/base.hpp>
#include <lydia/utils/misc.hpp>
#include <minisat/core/Solver.h>
#include <minisat/simp/SimpSolver.h>

namespace whitemech::lydia {

class PopulateSolverVisitor : Visitor {

private:
  Minisat::Solver &s;
  bool in_or = false;
  bool in_not = false;
  Minisat::vec<Minisat::Lit> current_clause;

public:
  std::map<atom_ptr, uint64_t, SharedComparator> atom2idx;
  std::vector<atom_ptr> idx2atom;
  std::vector<Minisat::Var> idx2var;

  /*
   * in_clause: switch behavior in case you are in a clause or not.
   */
  PopulateSolverVisitor(Minisat::Solver &s) : s{s} {}

  // callbacks for propositional logic
  void visit(const PropositionalTrue &) override;
  void visit(const PropositionalFalse &) override;
  void visit(const PropositionalAtom &) override;
  void visit(const PropositionalAnd &) override;
  void visit(const PropositionalOr &) override;
  void visit(const PropositionalNot &) override;

  void apply(const PropositionalFormula &b);
};

class SATModelEnumerationStategy : public ModelEnumerationStrategy {
private:
  Minisat::Solver solver_;

public:
  explicit SATModelEnumerationStategy() : solver_{} {}

  set_atoms_ptr extract_model(Minisat::Solver &solver,
                              const std::vector<atom_ptr> &idx2atom) {
    set_atoms_ptr result;
    // TODO could simplify the raw loop
    for (int i = 0; i < solver.model.size(); i++) {
      if (solver.model[i] == Minisat::l_True)
        result.insert(idx2atom[i]);
    }
    return result;
  }

  void ban_solution(Minisat::Solver &solver) {
    Minisat::vec<Minisat::Lit> clause;
    clause.clear();
    for (int i = 0; i < solver.nVars(); i++) {
      clause.push(Minisat::mkLit(i, solver.modelValue(i) == Minisat::l_True));
    }
    solver.addClause(clause);
  }

  std::vector<set_atoms_ptr> all_models(const PropositionalFormula &f) {
    std::vector<set_atoms_ptr> result;
    auto cnf_f = to_cnf(f);
    auto visitor = PopulateSolverVisitor(solver_);
    visitor.apply(*cnf_f);
    while (solver_.solve()) {
      auto model = extract_model(solver_, visitor.idx2atom);
      result.push_back(model);
      ban_solution(solver_);
    }
    return result;
  }
};

} // namespace whitemech::lydia