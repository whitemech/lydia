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

#include "visitor.hpp"
#include "logic.hpp"

namespace whitemech {
namespace lydia {
void Symbol::accept(Visitor &v) const { v.visit(*this); }
void LDLfBooleanAtom::accept(Visitor &v) const { v.visit(*this); }
void LDLfOr::accept(Visitor &v) const { v.visit(*this); }
void LDLfAnd::accept(Visitor &v) const { v.visit(*this); }
void LDLfNot::accept(Visitor &v) const { v.visit(*this); }
// TODO add other accept methods
} // namespace lydia
} // namespace whitemech
