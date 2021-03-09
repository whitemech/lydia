%option debug
%option prefix="ldlf"
%option yyclass="whitemech::lydia::parsers::ldlf::LDLfScanner"
%option noyywrap
%option c++

%{
/* C++ string header, for string ops below */
#include <string>

/* Implementation of yyFlexLDLfScanner */
#include "lydia/parser/ldlf/scanner_internal.hpp"
#undef  YY_DECL
#define YY_DECL int whitemech::lydia::parsers::ldlf::LDLfScanner::yylex( whitemech::lydia::parsers::ldlf::LDLf_YYSTYPE * lval, whitemech::lydia::parsers::ldlf::LDLfParser::location_type *loc )

/* typedef to make the returns for the tokens shorter */
using token = whitemech::lydia::parsers::ldlf::LDLfParser::token;

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

"tt"|"TT"                   { return token::TT; }
"ff"|"FF"                   { return token::FF; }

"true"|"TRUE"|"True"|"1"    { return token::TRUE_; }
"false"|"FALSE"|"False"|"0" { return token::FALSE_; }

"end"|"END"|"End"           { return token::END; }
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


