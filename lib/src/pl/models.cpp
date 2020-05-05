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
  auto sat_converter = SATConverter::create(f);
  while (true) {
    CMSat::lbool ret = sat_converter.solver.solve();
    if (ret != CMSat::l_True) {
      assert(ret == CMSat::l_False);
      // All solutions found.
      return models;
    }
    models.push_back(sat_converter.get_model());

    // Banning found solution
    std::vector<CMSat::Lit> ban_solution;
    for (uint32_t var = 0; var < sat_converter.solver.nVars(); var++) {
      if (sat_converter.solver.get_model()[var] != CMSat::l_Undef) {
        ban_solution.emplace_back(var, sat_converter.solver.get_model()[var] ==
                                           CMSat::l_True);
      }
    }
    sat_converter.solver.add_clause(ban_solution);
  }
}

std::vector<set_atoms_ptr> minimal_models(const PropositionalFormula &f) {
  auto models = all_models(f);
  std::vector<set_atoms_ptr> result;
  return models;
}

bool is_sat(const PropositionalFormula &f) {
  auto sat_converter = SATConverter::create(f);
  return sat_converter.solver.solve() == CMSat::l_True;
}

void SATConverter::visit(const PropositionalTrue &) { assert(false); }

void SATConverter::visit(const PropositionalFalse &) { assert(false); }

void SATConverter::visit(const PropositionalAtom &f) {
  auto x =
      std::static_pointer_cast<const PropositionalAtom>(f.shared_from_this());
  visit_atom(x, false);
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
  visit_atom(x, true);
}

void SATConverter::apply(const PropositionalFormula &f) { f.accept(*this); }

set_atoms_ptr SATConverter::get_model() {
  set_atoms_ptr result;
  for (uint32_t i = 0; i < solver.nVars(); i++) {
    if (solver.get_model()[i] == CMSat::l_True)
      result.insert(this->id2atom[i]);
  }
  return result;
}

SATConverter SATConverter::create(const PropositionalFormula &f) {
  auto converter = SATConverter();
  converter.apply(f);
  if (!is_a<PropositionalAnd>(f)) {
    // add clause
    converter.solver.add_clause(converter.clause);
  }
  return converter;
}

void SATConverter::visit_atom(const atom_ptr &f, bool is_inverted) {
  auto it = atom2id.find(f);
  uint32_t id;
  if (it == atom2id.end()) {
    id = atom2id.size();
    atom2id[f] = id;
    id2atom[id] = f;
    solver.new_var();
  } else {
    id = it->second;
  }
  clause.emplace_back(id, is_inverted);
}

} // namespace lydia
} // namespace whitemech