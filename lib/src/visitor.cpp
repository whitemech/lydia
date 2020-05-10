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

#include <lydia/logic.hpp>
#include <lydia/pl/logic.hpp>
#include <lydia/to_dfa/delta.hpp>
#include <lydia/visitor.hpp>

namespace whitemech {
namespace lydia {
void Symbol::accept(Visitor &v) const { v.visit(*this); }
void LDLfBooleanAtom::accept(Visitor &v) const { v.visit(*this); }
void LDLfOr::accept(Visitor &v) const { v.visit(*this); }
void LDLfAnd::accept(Visitor &v) const { v.visit(*this); }
void LDLfNot::accept(Visitor &v) const { v.visit(*this); }
void LDLfDiamond::accept(Visitor &v) const { v.visit(*this); }
void LDLfBox::accept(Visitor &v) const { v.visit(*this); }

void PropositionalRegExp::accept(Visitor &v) const { v.visit(*this); }
void TestRegExp::accept(Visitor &v) const { v.visit(*this); }
void UnionRegExp::accept(Visitor &v) const { v.visit(*this); }
void SequenceRegExp::accept(Visitor &v) const { v.visit(*this); }
void StarRegExp::accept(Visitor &v) const { v.visit(*this); }

// TODO add other accept methods

void PropositionalTrue::accept(Visitor &v) const { v.visit(*this); }

void PropositionalFalse::accept(Visitor &v) const { v.visit(*this); }
void PropositionalAtom::accept(Visitor &v) const { v.visit(*this); }

void PropositionalAnd::accept(Visitor &v) const { v.visit(*this); }
void PropositionalOr::accept(Visitor &v) const { v.visit(*this); }
void PropositionalNot::accept(Visitor &v) const { v.visit(*this); }

void LDLfF::accept(Visitor &v) const { v.visit(*this); }
void LDLfT::accept(Visitor &v) const { v.visit(*this); }
void QuotedFormula::accept(Visitor &v) const { v.visit(*this); }

} // namespace lydia
} // namespace whitemech
