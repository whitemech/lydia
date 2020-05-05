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
std::vector<set_atoms_ptr> all_models_sat(const PropositionalFormula &f);

/*!
 * Compute the minimal models of a propositional formula.
 *
 * @param f the propositional formula
 * @return the set of minimal models.
 */
std::vector<set_atoms_ptr> minimal_models_sat(const PropositionalFormula &f);

/*
 * Check whether the formula is satisfiable.
 */
bool is_sat(const PropositionalFormula &f);

class SATConverter : public Visitor {
private:
  std::vector<CMSat::Lit> clause;
  SATConverter() { solver.set_num_threads(4); }
  void apply(const PropositionalFormula &);

public:
  CMSat::SATSolver solver;
  std::map<atom_ptr, uint32_t, SharedComparator> atom2id;
  std::map<uint32_t, atom_ptr> id2atom;

  set_atoms_ptr get_model();
  bool is_sat();

  void visit(const PropositionalTrue &) override;
  void visit(const PropositionalFalse &) override;
  void visit(const PropositionalAtom &) override;
  void visit(const PropositionalAnd &) override;
  void visit(const PropositionalOr &) override;
  void visit(const PropositionalNot &) override;

  void visit_atom(const atom_ptr &, bool is_inverted);
  static SATConverter create(const PropositionalFormula &f);
};

} // namespace lydia
} // namespace whitemech