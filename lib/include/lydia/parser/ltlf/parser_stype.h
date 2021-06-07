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

#include "lydia/logic/ltlf/base.hpp"
#include <string>

namespace whitemech::lydia::parsers::ltlf {

struct LTLf_YYSTYPE {
  ldlf_ptr formula;
  std::string symbol_name;

  // Constructor
  LTLf_YYSTYPE() = default;
  // Destructor
  ~LTLf_YYSTYPE() = default;
  // Copy constructor and assignment
  LTLf_YYSTYPE(const LTLf_YYSTYPE&) = default;
  LTLf_YYSTYPE& operator=(const LTLf_YYSTYPE&) = default;
  // Move constructor and assignment
  LTLf_YYSTYPE(LTLf_YYSTYPE&&) = default;
  LTLf_YYSTYPE& operator=(LTLf_YYSTYPE&&) = default;
};

} // namespace whitemech::lydia::parsers::ltlf
