%option debug
%option prefix="ltlf"
%option yyclass="whitemech::lydia::parsers::ltlf::LTLfScanner"
%option noyywrap
%option c++

%{
/* C++ string header, for string ops below */
#include <string>

/* Implementation of yyFlexLTLfScanner */
#include "lydia/parser/ltlf/scanner_internal.hpp"
#undef  YY_DECL
#define YY_DECL int whitemech::lydia::parsers::ltlf::LTLfScanner::yylex( whitemech::lydia::parsers::ltlf::LTLf_YYSTYPE * lval, whitemech::lydia::parsers::ltlf::LTLfParser::location_type *loc )

/* typedef to make the returns for the tokens shorter */
using token = whitemech::lydia::parsers::ltlf::LTLfParser::token;

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

"true"|"TRUE"|"True"|"1"    { return token::TRUE_; }
"false"|"FALSE"|"False"|"0" { return token::FALSE_; }

"last"|"LAST"|"Last"        { return token::LAST; }

[a-zA-Z_][a-zA-Z0-9_]*|("[a-zA-Z0-9_]+") {
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


