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
#include "pl/eval.hpp"
#include "pl/logic.hpp"
#include "utils/misc.hpp"
#include <atom_visitor.hpp>
#include <cryptominisat5/cryptominisat.h>

namespace whitemech {
namespace lydia {

void EvalVisitor::visit(const PropositionalTrue &) { result = true; }
void EvalVisitor::visit(const PropositionalFalse &) { result = false; }

void EvalVisitor::visit(const PropositionalAtom &a) {
  for (const auto &x : interpretation) {
    if (*x == a) {
      result = true;
      return;
    }
  }
  result = false;
}

void EvalVisitor::visit(const PropositionalNot &a) {
  result = not apply(*a.get_arg());
}

void EvalVisitor::visit(const PropositionalAnd &a) {
  result = true;
  for (const auto &subformula : a.get_container()) {
    if (!apply(*subformula)) {
      result = false;
      return;
    }
  }
}

void EvalVisitor::visit(const PropositionalOr &a) {
  result = false;
  for (const auto &subformula : a.get_container()) {
    if (apply(*subformula)) {
      result = true;
      return;
    }
  }
}

bool EvalVisitor::apply(const PropositionalFormula &b) {
  b.accept(*this);
  return result;
}

bool eval(const PropositionalFormula &f, const set_atoms_ptr &interpretation) {
  EvalVisitor evalVisitor{interpretation};
  return evalVisitor.apply(f);
}

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

  // Let's use 4 threads
  solver.set_num_threads(4);

  auto atoms = find_atoms(f);
  solver.new_vars(atoms.size());

  return false;
}

} // namespace lydia
} // namespace whitemech
