%{
/* C++ string header, for string ops below */
#include <string>
#include "parser_stype.h"

/* Implementation of yyFlexScanner */
#include "scanner.hpp"
#undef  YY_DECL
#define YY_DECL int whitemech::lydia::Scanner::yylex( whitemech::lydia::YYSTYPE * lval, whitemech::lydia::Parser::location_type *loc )

/* typedef to make the returns for the tokens shorter */
using token = whitemech::lydia::Parser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option debug
%option nodefault
%option yyclass="whitemech::lydia::Scanner"
%option noyywrap
%option c++

%%

%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

"&&"|"&"    { return token::AND; }

"||"|"|"    { return token::OR; }

"!"|"~"     { return token::NOT; }

(?i:tt)     { return token::TT; }

(?i:ff)     { return token::FF; }

\n          {
               // Update line number
               loc->lines();
               return token::NEWLINE;
            }

%%


