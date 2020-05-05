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

#include "pl/models.hpp"
#include <pl/cnf.hpp>

namespace whitemech {
namespace lydia {

std::vector<set_atoms_ptr> all_models(const PropositionalFormula &f) {
  std::vector<set_atoms_ptr> models;
  auto all_atoms = find_atoms(f);
  std::vector<set_atoms_ptr> all_interpretations =
      powerset<atom_ptr, SharedComparator>(all_atoms);
  for (set_atoms_ptr &interpretation : all_interpretations) {
    if (eval(f, interpretation)) {
      models.emplace_back(interpretation);
    }
  }
  return models;
}

std::vector<set_atoms_ptr> minimal_models(const PropositionalFormula &f) {
  auto models = all_models(f);
  std::vector<set_atoms_ptr> result;
  return models;
}

bool is_sat(const PropositionalFormula &f) {
  CMSat::SATSolver solver;
  std::vector<CMSat::Lit> clause;
  solver.set_num_threads(4);

  auto f_cnf = to_cnf(f);
  auto atoms = find_atoms(*f_cnf);

  solver.new_vars(atoms.size());
  //    for (const auto& prop_clause : f_cnf){
  //
  //    }
  //

  return false;
}
} // namespace lydia
} // namespace whitemech