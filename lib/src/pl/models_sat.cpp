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

#include <lydia/pl/cnf.hpp>
#include <lydia/pl/models_sat.hpp>
#include <numeric>

namespace whitemech {
namespace lydia {

bool SATSolverWrapper::is_sat() {
  return this->get_solver().solve() == CMSat::l_True;
}

std::vector<set_atoms_ptr> all_models_sat(const PropositionalFormula &f) {
  std::vector<set_atoms_ptr> models;
  auto sat_converter = SATSolverWrapper::create(f);
  auto solver = sat_converter.get_solver();

  while (true) {
    CMSat::lbool ret = solver.solve();
    if (ret != CMSat::l_True) {
      assert(ret == CMSat::l_False);
      // All solutions found.
      return models;
    }
    models.push_back(sat_converter.get_model(solver.get_model()));

    // Banning found solution
    std::vector<CMSat::Lit> ban_solution;
    for (uint32_t var = 0; var < solver.nVars(); var++) {
      if (solver.get_model()[var] != CMSat::l_Undef) {
        ban_solution.emplace_back(var,
                                  solver.get_model()[var] == CMSat::l_True);
      }
    }
    solver.add_clause(ban_solution);
  }
}

set_atoms_ptr minimal_model_sat(const PropositionalFormula &f) {
  set_atoms_ptr result;
  auto converter = SATSolverWrapper::create(f);
  return result;
}

std::vector<set_atoms_ptr>
all_minimal_models_sat(const PropositionalFormula &f) {
  std::vector<set_atoms_ptr> models;
  auto sat_converter = SATSolverWrapper::create(f);

  while (true) {
    auto model = sat_converter.find_minimal_model();
    if (!model.has_value()) {
      return models;
    }
    models.push_back(sat_converter.get_model(model.value()));

    // Banning found solution
    std::vector<CMSat::Lit> ban_solution;
    for (const auto &i : model.value()) {
      ban_solution.emplace_back(i, true);
    }
    sat_converter.clauses.emplace_back(ban_solution);
  }
}

bool is_sat(const PropositionalFormula &f) {
  auto sat_converter = SATSolverWrapper::create(f);
  return sat_converter.get_solver().solve() == CMSat::l_True;
}

std::optional<std::vector<uint32_t>> SATSolverWrapper::find_minimal_model() {
  if (this->get_solver().solve() != CMSat::l_True) {
    return std::nullopt;
  }
  auto b = std::vector<uint32_t>();
  auto target = std::vector<uint32_t>(this->id2atom.size());
  std::iota(target.begin(), target.end(), 0);
  auto model = quickxplain(b, target, true);
  return model;
}

void SATSolverWrapper::visit(const PropositionalTrue &) {}

void SATSolverWrapper::visit(const PropositionalFalse &) {}

void SATSolverWrapper::visit(const PropositionalAtom &f) {
  auto x =
      std::static_pointer_cast<const PropositionalAtom>(f.shared_from_this());
  visit_atom(x, false);
}

void SATSolverWrapper::visit(const PropositionalAnd &f) {
  for (const auto &subformula : f.get_container()) {
    clause.clear();
    apply(*subformula);
    clauses.emplace_back(clause);
  }
}

void SATSolverWrapper::visit(const PropositionalOr &f) {
  for (const auto &subformula : f.get_container()) {
    apply(*subformula);
  }
}

void SATSolverWrapper::visit(const PropositionalNot &f) {
  auto x = std::static_pointer_cast<const PropositionalAtom>(f.get_arg());
  visit_atom(x, true);
}

void SATSolverWrapper::apply(const PropositionalFormula &f) { f.accept(*this); }

set_atoms_ptr
SATSolverWrapper::get_model(const std::vector<CMSat::lbool> &model) {
  set_atoms_ptr result;
  for (uint32_t i = 0; i < id2atom.size(); i++) {
    if (model[i] == CMSat::l_True)
      result.insert(this->id2atom[i]);
  }
  return result;
}
set_atoms_ptr SATSolverWrapper::get_model(const std::vector<uint32_t> &model) {
  set_atoms_ptr result;
  for (const auto &i : model) {
    result.insert(this->id2atom[i]);
  }
  return result;
}

SATSolverWrapper SATSolverWrapper::create(const PropositionalFormula &f) {
  auto converter = SATSolverWrapper();
  converter.apply(f);
  if (is_a<PropositionalFalse>(f)) {
    converter.clause.clear();
    converter.clauses.emplace_back(converter.clause);
  } else if (!is_a<PropositionalAnd>(f) and !is_a<PropositionalTrue>(f)) {
    // add clause
    converter.clauses.emplace_back(converter.clause);
  }
  converter.varset = std::vector<uint32_t>(converter.id2atom.size());
  std::iota(converter.varset.begin(), converter.varset.end(), 0);
  return converter;
}

void SATSolverWrapper::visit_atom(const atom_ptr &f, bool is_inverted) {
  auto it = atom2id.find(f);
  uint32_t id;
  if (it == atom2id.end()) {
    id = atom2id.size();
    atom2id[f] = id;
    id2atom[id] = f;
  } else {
    id = it->second;
  }
  clause.emplace_back(id, is_inverted);
}

CMSat::SATSolver SATSolverWrapper::get_solver() {
  auto solver = CMSat::SATSolver();
  solver.set_num_threads(4);
  solver.new_vars(this->id2atom.size());
  for (const auto &clause_ : this->clauses) {
    solver.add_clause(clause_);
  }
  return solver;
}

bool SATSolverWrapper::predicate(const std::vector<uint32_t> &v) {
  auto solver = this->get_solver();
  std::vector<uint32_t> negated_atoms;
  std::set_difference(varset.begin(), varset.end(), v.begin(), v.end(),
                      std::inserter(negated_atoms, negated_atoms.begin()));
  for (const auto &x : negated_atoms) {
    solver.add_clause(std::vector<CMSat::Lit>{CMSat::Lit(x, true)});
  }
  return solver.solve() == CMSat::l_True;
}

std::vector<uint32_t> SATSolverWrapper::quickxplain(std::vector<uint32_t> &b,
                                                    std::vector<uint32_t> &t,
                                                    bool has_set) {
  if (has_set and this->predicate(b)) {
    return std::vector<uint32_t>();
  }
  if (t.size() == 1) {
    return t;
  }
  size_t m = t.size() / 2;
  std::vector<uint32_t> t1, t2;
  t1 = std::vector<uint32_t>(t.begin(), t.begin() + m);
  t2 = std::vector<uint32_t>(t.begin() + m, t.end());
  assert(t1.size() + t2.size() == t.size());

  //    no need to sort
  //    std::sort(t1.begin(), t1.end());
  //    std::sort(t2.begin(), t2.end());

  auto b_union_t1 = set_union(b, t1);
  auto m2 = quickxplain(b_union_t1, t2, not t1.empty());

  auto b_union_m2 = set_union(b, m2);
  auto m1 = quickxplain(b_union_m2, t1, not m2.empty());

  auto m1_union_m2 = set_union(m1, m2);
  return m1_union_m2;
}

} // namespace lydia
} // namespace whitemech