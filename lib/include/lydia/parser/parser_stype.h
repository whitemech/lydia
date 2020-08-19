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

#include "lydia/ldlf/logic.hpp"
#include "lydia/pl/logic.hpp"
#include <string>

namespace whitemech {
namespace lydia {

struct YYSTYPE {
  std::shared_ptr<const LDLfFormula> formula;
  std::shared_ptr<const RegExp> regex;
  std::shared_ptr<const PropositionalFormula> prop_formula;
  std::string symbol_name;

  // Constructor
  YYSTYPE() = default;
  // Destructor
  ~YYSTYPE() = default;
  // Copy constructor and assignment
  YYSTYPE(const YYSTYPE &) = default;
  YYSTYPE &operator=(const YYSTYPE &) = default;
  // Move constructor and assignment
  YYSTYPE(YYSTYPE &&) = default;
  YYSTYPE &operator=(YYSTYPE &&) = default;
};

} // namespace lydia
} // namespace whitemech
