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
  SATConverter solver;
  solver.apply(f);
  return solver.solver.solve() == CMSat::l_True;
}

void SATConverter::visit(const PropositionalTrue &) { assert(false); }

void SATConverter::visit(const PropositionalFalse &) { assert(false); }

void SATConverter::visit(const PropositionalAtom &f) {
  auto x =
      std::static_pointer_cast<const PropositionalAtom>(f.shared_from_this());
  auto it = atom2id.find(x);
  if (it == atom2id.end()) {
    auto id = atom2id.size();
    atom2id[x] = id;
    id2atom[id] = x;
    solver.new_var();
  }
  clause.emplace_back(it->second, false);
}

void SATConverter::visit(const PropositionalAnd &f) {
  for (const auto &subformula : f.get_container()) {
    clause.clear();
    apply(*subformula);
    solver.add_clause(clause);
  }
}

void SATConverter::visit(const PropositionalOr &f) {
  for (const auto &subformula : f.get_container()) {
    apply(*subformula);
  }
}

void SATConverter::visit(const PropositionalNot &f) {
  auto x = std::static_pointer_cast<const PropositionalAtom>(f.get_arg());
  auto it = atom2id.find(x);
  if (it == atom2id.end()) {
    auto id = atom2id.size();
    atom2id[x] = id;
    id2atom[id] = x;
    solver.new_var();
  }
  clause.emplace_back(it->second, true);
}

void SATConverter::apply(const PropositionalFormula &f) { f.accept(*this); }

} // namespace lydia
} // namespace whitemech