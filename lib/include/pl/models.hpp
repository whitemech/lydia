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

#include "eval.hpp"
#include <atom_visitor.hpp>
#include <cryptominisat5/cryptominisat.h>
#include <utils/misc.hpp>

namespace whitemech {
namespace lydia {

/*!
 * Compute all the models of a propositional formula.
 *
 * @param f the propositional formula
 * @return the set of the models of a formula
 */
std::vector<set_atoms_ptr> all_models(const PropositionalFormula &f);

/*!
 * Compute the minimal models of a propositional formula.
 *
 * @param f the propositional formula
 * @return the set of minimal models.
 */
std::vector<set_atoms_ptr> minimal_models(const PropositionalFormula &f);

/*
 * Check whether the formula is satisfiable.
 */
bool is_sat(const PropositionalFormula &f);

CMSat::SATSolver from_propositional_formula(const PropositionalFormula &f);

class SATConverter : public Visitor {
public:
  CMSat::SATSolver solver;
  std::vector<CMSat::Lit> clause;
  std::map<prop_ptr, int, SharedComparator> atom2id;
  std::map<int, prop_ptr> id2atom;

  SATConverter() {
    solver.set_num_threads(4);
    // add dummy symbol to encode true and false
  }

  void visit(const PropositionalTrue &) override;
  void visit(const PropositionalFalse &) override;
  void visit(const PropositionalAtom &) override;
  void visit(const PropositionalAnd &) override;
  void visit(const PropositionalOr &) override;
  void visit(const PropositionalNot &) override;

  void apply(const PropositionalFormula &);
};

std::vector<CMSat::Lit> get_clauses();

} // namespace lydia
} // namespace whitemech