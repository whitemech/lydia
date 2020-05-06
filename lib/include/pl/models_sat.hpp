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

/*
 * Check whether the formula is satisfiable.
 */
bool is_sat(const PropositionalFormula &f);

class SATSolverWrapper : public Visitor {
private:
  std::vector<CMSat::Lit> clause;
  void apply(const PropositionalFormula &);

public:
  std::vector<std::vector<CMSat::Lit>> clauses;
  std::map<atom_ptr, uint32_t, SharedComparator> atom2id;
  std::map<uint32_t, atom_ptr> id2atom;
  std::vector<uint32_t> varset;

  set_atoms_ptr get_model(const std::vector<CMSat::lbool> &model);
  set_atoms_ptr get_model(const std::vector<uint32_t> &model);
  CMSat::SATSolver get_solver();
  bool is_sat();

  std::optional<std::vector<uint32_t>> find_minimal_model();
  std::vector<uint32_t> quickxplain(std::vector<uint32_t> &,
                                    std::vector<uint32_t> &, bool has_set);
  bool predicate(const std::vector<uint32_t> &v);

  void visit(const PropositionalTrue &) override;
  void visit(const PropositionalFalse &) override;
  void visit(const PropositionalAtom &) override;
  void visit(const PropositionalAnd &) override;
  void visit(const PropositionalOr &) override;
  void visit(const PropositionalNot &) override;

  void visit_atom(const atom_ptr &, bool is_inverted);
  static SATSolverWrapper create(const PropositionalFormula &f);
};

std::vector<set_atoms_ptr> all_models_sat(const PropositionalFormula &f);

std::vector<set_atoms_ptr>
all_minimal_models_sat(const PropositionalFormula &f);

set_atoms_ptr minimal_model_sat(const PropositionalFormula &f);

} // namespace lydia
} // namespace whitemech