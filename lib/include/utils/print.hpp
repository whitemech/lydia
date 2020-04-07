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

#include "visitor.hpp"

namespace whitemech {
namespace lydia {

class StrPrinter : public BaseVisitor<StrPrinter> {
private:
  static const std::vector<std::string> names_;

protected:
  std::string str_;

public:
  void bvisit(const Basic &x);
  void bvisit(const Symbol &x);
  void bvisit(const LDLfBooleanAtom &x);
  void bvisit(const LDLfAnd &x);
  void bvisit(const LDLfOr &x);
  void bvisit(const LDLfNot &x);
  std::string apply(const vec_basic &v);
  std::string apply(const set_formulas &v);
  std::string apply(const Basic &b);
};

std::string str(const Basic &x);

} // namespace lydia
} // namespace whitemech