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

#include "cuddObj.hh"
#include "propositional_logic.hpp"
#include "visitor.hpp"

namespace whitemech {
namespace lydia {

/*!
 * This visitor implements the 'delta' function of [1].
 *
 * [1] Brafman, Ronen I., Giuseppe De Giacomo, and Fabio Patrizi.
 *     "LTLf/LDLf non-markovian rewards."
 *     Thirty-Second AAAI Conference on Artificial Intelligence. 2018.
 */

class QuotedFormula : public Basic {
private:
protected:
public:
  const LDLfFormula &formula;
  /*!
   * Quote an LDLf formula. We assume it is in NNF.
   * @param f: the LDLf formula.
   */
  explicit QuotedFormula(const LDLfFormula &formula) : formula{formula} {}
};

class DeltaVisitor : public Visitor {
private:
protected:
  std::shared_ptr<const PropositionalFormula> result;
  bool epsilon;

public:
  explicit DeltaVisitor(bool epsilon = false) : epsilon{epsilon} {}

  void visit(const Symbol &) override{};
  void visit(const LDLfBooleanAtom &) override;
  void visit(const LDLfAnd &) override;
  void visit(const LDLfOr &) override;
  void visit(const LDLfNot &) override{};
  std::shared_ptr<const PropositionalFormula> apply(const LDLfFormula &b);
};

/*!
 * Wrapper to the visitor.
 *
 * @param epsilon the epsilon parameter to the delta function.
 * @return the propositional logic formula with atoms as quoted LDLf formulas.
 *        | In the case epsilon is true, always return either True or False.
 */
/*TODO the last bit implies doing some simplification of the Ands/Ors, or
 * doing some post-processing (e.g. evaluate the formula with a "dummy model").
 */
std::shared_ptr<const PropositionalFormula> delta(const LDLfFormula &,
                                                  bool epsilon = false);

} // namespace lydia
} // namespace whitemech