%option debug
%option prefix="ltlf"
%option yyclass="whitemech::lydia::LTLfScanner"
%option noyywrap
%option c++

%{
/* C++ string header, for string ops below */
#include <string>

/* Implementation of yyFlexLTLfScanner */
#include "lydia/parser/ltlf/scanner_internal.hpp"
#undef  YY_DECL
#define YY_DECL int whitemech::lydia::LTLfScanner::yylex( whitemech::lydia::LTLf_YYSTYPE * lval, whitemech::lydia::LTLfParser::location_type *loc )

/* typedef to make the returns for the tokens shorter */
using token = whitemech::lydia::LTLfParser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END_OF_FILE )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}


%%

%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

"("                         { return token::LPAR; }
")"                         { return token::RPAR; }

"X"                         { return token::NEXT; }
"W"                         { return token::WEAK_NEXT; }
"U"                         { return token::UNTIL; }
"R"                         { return token::RELEASE; }
"F"                         { return token::EVENTUALLY; }
"G"                         { return token::ALWAYS; }

"<=>"|"<->"                 { return token::EQUIVALENCE; }
"=>"|"->"                   { return token::IMPLICATION; }

"&&"|"&"                    { return token::AND; }
"||"|"|"                    { return token::OR; }

"!"|"~"                     { return token::NOT; }

(?i:true)                   { return token::TRUE_; }
(?i:false)                  { return token::FALSE_; }

(?i:last)                   { return token::LAST; }

[a-zA-Z_][a-zA-Z0-9_]*|(".*") {
                                yylval->symbol_name = yytext;
                                return token::SYMBOL;
                            }

\n                          {
                               // Update line number
                               loc->lines();
                               return token::NEWLINE;
                            }
[\t\r\n ]+                  { ; }

%%

