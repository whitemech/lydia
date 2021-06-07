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

#include "lydia/parser/ldlf/location.hh"
#include "lydia/parser/ldlf/parser.tab.hh"
#include <lydia/parser/ldlf/parser_stype.h>

namespace whitemech::lydia::parsers::ldlf {

class LDLfScanner : public ldlfFlexLexer {
public:
  /* yyval ptr */
  whitemech::lydia::parsers::ldlf::LDLf_YYSTYPE* yylval = nullptr;

  explicit LDLfScanner(std::istream* in) : ldlfFlexLexer(in){};
  virtual ~LDLfScanner(){};

  // get rid of override virtual function warning
  using FlexLexer::yylex;

  virtual int yylex(whitemech::lydia::parsers::ldlf::LDLf_YYSTYPE* lval,
                    LDLfParser::location_type* location);
  // YY_DECL defined in lexer.l
  // Method body created by flex in lexer.yy.cc
};

} // namespace whitemech::lydia::parsers::ldlf
