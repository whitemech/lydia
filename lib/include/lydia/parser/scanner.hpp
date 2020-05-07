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

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include <lydia/parser/location.hh>
#include <lydia/parser/parser.tab.hh>
#include <lydia/parser/parser_stype.h>

namespace whitemech {
namespace lydia {

class Scanner : public yyFlexLexer {
private:
public:
  /* yyval ptr */
  whitemech::lydia::YYSTYPE *yylval = nullptr;

  explicit Scanner(std::istream *in) : yyFlexLexer(in){};
  virtual ~Scanner(){};

  // get rid of override virtual function warning
  using FlexLexer::yylex;

  virtual int yylex(whitemech::lydia::YYSTYPE *lval,
                    Parser::location_type *location);
  // YY_DECL defined in lexer.l
  // Method body created by flex in lexer.yy.cc
};

} // namespace lydia
} // namespace whitemech
