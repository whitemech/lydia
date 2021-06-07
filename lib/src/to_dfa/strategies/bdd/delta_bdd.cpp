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

#include <lydia/to_dfa/strategies/bdd/base.hpp>
#include <lydia/to_dfa/strategies/bdd/delta_bdd.hpp>

namespace whitemech::lydia {

void BDDDeltaSymbolicVisitor::visit(const PropositionalTrue& f) {
  result = this->s.mgr.bddOne();
};
void BDDDeltaSymbolicVisitor::visit(const PropositionalFalse& f) {
  result = this->s.mgr.bddZero();
};
void BDDDeltaSymbolicVisitor::visit(const PropositionalAtom& f) {
  if (is_a<Symbol>(*f.symbol)) {
    auto ptr =
        std::static_pointer_cast<const PropositionalAtom>(f.shared_from_this());
    size_t var_id = this->s.atom2ids[ptr];
    result = this->s.automaton->bddvars[this->s.automaton->nb_bits + var_id];
  } else {
    assert(is_a<QuotedFormula>(*f.symbol));
    auto quoted_ldlf_formula = std::static_pointer_cast<const LDLfFormula>(
        dynamic_cast<const QuotedFormula&>(*f.symbol).formula);
    auto it = this->s.subformula2id.find(quoted_ldlf_formula);
    if (it == this->s.subformula2id.end()) {
      size_t index = this->s.subformula2id.size();
      this->s.subformula2id[quoted_ldlf_formula] = index;
      this->s.id2subformula.push_back(quoted_ldlf_formula);
      CUDD::BDD var = this->s.mgr.bddVar();
      this->s.subformula_bddvars.push_back(var);

      result = var;
    } else {
      result = this->s.subformula_bddvars[it->second];
    }
  }
};
void BDDDeltaSymbolicVisitor::visit(const PropositionalAnd& f) {
  CUDD::BDD tmp = this->s.mgr.bddOne();
  for (const auto& x : f.get_args()) {
    tmp &= apply(*x);
  }
  result = tmp;
};
void BDDDeltaSymbolicVisitor::visit(const PropositionalOr& f) {
  CUDD::BDD tmp = this->s.mgr.bddZero();
  for (const auto& x : f.get_args()) {
    tmp += apply(*x);
  }
  result = tmp;
};
void BDDDeltaSymbolicVisitor::visit(const PropositionalNot& f) {
  CUDD::BDD tmp = apply(*f.get_arg());
  result = !tmp;
};

CUDD::BDD bdd_delta_symbolic(BDDStrategy& s, const PropositionalFormula& f,
                             bool epsilon) {
  auto visitor = BDDDeltaSymbolicVisitor(s, epsilon);
  return visitor.apply(f);
}

} // namespace whitemech::lydia