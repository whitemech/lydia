%option debug
%option prefix="ldlf"
%option yyclass="whitemech::lydia::LDLfScanner"
%option noyywrap
%option c++

%{
/* C++ string header, for string ops below */
#include <string>

/* Implementation of yyFlexLDLfScanner */
#include "lydia/parser/ldlf/scanner_internal.hpp"
#undef  YY_DECL
#define YY_DECL int whitemech::lydia::LDLfScanner::yylex( whitemech::lydia::LDLf_YYSTYPE * lval, whitemech::lydia::LDLfParser::location_type *loc )

/* typedef to make the returns for the tokens shorter */
using token = whitemech::lydia::LDLfParser::token;

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

"["                         { return token::BOX_LPAR; }
"]"                         { return token::BOX_RPAR; }

"<"                         { return token::DIAMOND_LPAR; }
">"                         { return token::DIAMOND_RPAR; }

"+"                         { return token::UNION; }
";"                         { return token::SEQUENCE; }
"?"                         { return token::TEST; }
"*"                         { return token::STAR; }

"<=>"|"<->"                 { return token::EQUIVALENCE; }
"=>"|"->"                   { return token::IMPLICATION; }

"&&"|"&"                    { return token::AND; }
"||"|"|"                    { return token::OR; }

"!"|"~"                     { return token::NOT; }

(?i:tt)                     { return token::TT; }
(?i:ff)                     { return token::FF; }

(?i:true)                   { return token::TRUE_; }
(?i:false)                  { return token::FALSE_; }

(?i:end)                    { return token::END; }
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


