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

#include "utils/print.hpp"
#include "types.hpp"
#include <stdexcept>

namespace whitemech {
namespace lydia {

std::string StrPrinter::apply(const Basic &b) {
  b.accept(*this);
  return str_;
}

void StrPrinter::bvisit(const Basic &x) { throw not_implemented_error(); }

void StrPrinter::bvisit(const Symbol &x) {}

void StrPrinter::bvisit(const LDLfBooleanAtom &x) {}

void StrPrinter::bvisit(const LDLfAnd &x) {}

void StrPrinter::bvisit(const LDLfOr &x) {}

void StrPrinter::bvisit(const LDLfNot &x) {}

std::string apply(const vec_basic &v) {}

std::string apply(const set_formulas &v) {}

std::string apply(const Basic &b) {}

std::string str(const Basic &x) {
  StrPrinter strPrinter;
  return strPrinter.apply(x);
}

} // namespace lydia
} // namespace whitemech