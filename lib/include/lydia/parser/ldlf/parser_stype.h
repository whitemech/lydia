#pragma once
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

#include "lydia/logic/ldlf/base.hpp"
#include "lydia/logic/pl/base.hpp"
#include <string>

namespace whitemech::lydia::parsers::ldlf {

struct LDLf_YYSTYPE {
  std::shared_ptr<const LDLfFormula> formula;
  std::shared_ptr<const RegExp> regex;
  std::shared_ptr<const PropositionalFormula> prop_formula;
  std::string symbol_name;

  // Constructor
  LDLf_YYSTYPE() = default;
  // Destructor
  ~LDLf_YYSTYPE() = default;
  // Copy constructor and assignment
  LDLf_YYSTYPE(const LDLf_YYSTYPE&) = default;
  LDLf_YYSTYPE& operator=(const LDLf_YYSTYPE&) = default;
  // Move constructor and assignment
  LDLf_YYSTYPE(LDLf_YYSTYPE&&) = default;
  LDLf_YYSTYPE& operator=(LDLf_YYSTYPE&&) = default;
};

} // namespace whitemech::lydia::parsers::ldlf
