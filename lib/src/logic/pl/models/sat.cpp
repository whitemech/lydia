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

#include <lydia/logic/pl/cnf.hpp>
#include <lydia/logic/pl/models/base.hpp>
#include <lydia/logic/pl/models/sat.hpp>

namespace whitemech::lydia {

void PopulateSolverVisitor::visit(const PropositionalTrue &) {
  if (in_or) {
    // TODO the true helps only if in the or. We should simplify the clause in
    //  this case.
  }
}
void PopulateSolverVisitor::visit(const PropositionalFalse &) {
  if (not in_or) {
    // we must be in the and... add empty clause
    s.addEmptyClause();
  }
}
void PopulateSolverVisitor::visit(const PropositionalAtom &f) {
  atom_ptr f_ptr =
      std::static_pointer_cast<const PropositionalAtom>(f.shared_from_this());
  auto var_index = atom2idx.find(f_ptr);
  int index = -1;
  if (var_index == atom2idx.end()) {
    auto new_var = s.newVar();
    index = idx2var.size();
    idx2var.push_back(new_var);
    idx2atom.push_back(f_ptr);
    atom2idx[f_ptr] = index;
  } else {
    index = var_index->second;
  }
  Minisat::Lit literal = Minisat::mkLit(idx2var[index], in_not);
  if (in_or)
    current_clause.push(literal);
  else
    s.addClause(literal);
}
void PopulateSolverVisitor::visit(const PropositionalAnd &f) {
  for (const auto &subf : f.get_container()) {
    subf->accept(*this);
  }
}
void PopulateSolverVisitor::visit(const PropositionalOr &f) {
  // formula must be in CNF
  assert(!in_or);
  assert(!in_not);
  in_or = true;
  current_clause.clear();
  for (const auto &subf : f.get_container())
    subf->accept(*this);
  s.addClause(current_clause);
  in_or = false;
}
void PopulateSolverVisitor::visit(const PropositionalNot &f) {
  in_not = true;
  f.get_arg()->accept(*this);
  in_not = false;
}

void PopulateSolverVisitor::apply(const PropositionalFormula &b) {
  b.accept(*this);
}

} // namespace whitemech::lydia