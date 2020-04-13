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

#include "delta.hpp"
#include "visitor.hpp"
#include <variant>

namespace whitemech {
namespace lydia {

class AtomsVisitor : public Visitor {
private:
protected:
  std::set<std::shared_ptr<PropositionalAtom>> result;

public:
  void visit(const PropositionalTrue &) override;
  void visit(const PropositionalFalse &) override;
  void visit(const PropositionalAtom &) override;
  void visit(const QuotedFormula &) override;
  void visit(const PropositionalAnd &) override;
  void visit(const PropositionalOr &) override;
  void visit(const PropositionalNot &) override;
  std::set<std::shared_ptr<PropositionalAtom>>
  apply(const PropositionalFormula &b);
};

std::shared_ptr<LDLfFormula> to_nnf(const LDLfFormula &);

} // namespace lydia
} // namespace whitemech