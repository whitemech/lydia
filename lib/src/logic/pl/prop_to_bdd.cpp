/*
 * This file is part of Lydia.
 *
 * Lydia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Lydia is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lydia.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <lydia/logger.hpp>
#include <lydia/logic/nnf.hpp>
#include <lydia/logic/pl/base.hpp>
#include <lydia/logic/pl/prop_to_bdd.hpp>

namespace whitemech::lydia {

CUDD::BDD PropToBDDVisitor::apply(const PropositionalFormula& b) {
  b.accept(*this);
  return result;
}

void PropToBDDVisitor::visit(const PropositionalTrue& f) {
  result = this->mgr->bddOne();
}
void PropToBDDVisitor::visit(const PropositionalFalse& f) {
  result = this->mgr->bddZero();
}
void PropToBDDVisitor::visit(const PropositionalAtom& f) {
  auto ptr =
      std::static_pointer_cast<const PropositionalAtom>(f.shared_from_this());
  size_t var_id = atom2id[ptr];
  result = mgr->bddVar(var_id);
}

void PropToBDDVisitor::visit(const PropositionalAnd& f) {
  CUDD::BDD tmp = this->mgr->bddOne();
  for (const auto& x : f.get_args()) {
    tmp &= apply(*x);
  }
  result = tmp;
}
void PropToBDDVisitor::visit(const PropositionalOr& f) {
  CUDD::BDD tmp = this->mgr->bddZero();
  for (const auto& x : f.get_args()) {
    tmp += apply(*x);
  }
  result = tmp;
}
void PropToBDDVisitor::visit(const PropositionalNot& f) {
  CUDD::BDD tmp = apply(*f.get_arg());
  result = !tmp;
}

} // namespace whitemech::lydia