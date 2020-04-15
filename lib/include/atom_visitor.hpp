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
  set_atoms_ptr result;

public:
  static Logger logger;

  void visit(const PropositionalTrue &) override;
  void visit(const PropositionalFalse &) override;
  void visit(const PropositionalAtom &) override;
  void visit(const QuotedFormula &) override{};
  void visit(const Symbol &) override{};
  void visit(const PropositionalAnd &) override;
  void visit(const PropositionalOr &) override;
  void visit(const PropositionalNot &) override;
  set_atoms_ptr apply(const PropositionalFormula &b);
};

set_atoms_ptr find_atoms(const LDLfFormula &);
set_atoms_ptr find_atoms(const PropositionalFormula &);

} // namespace lydia
} // namespace whitemech