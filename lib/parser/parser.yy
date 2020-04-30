%skeleton "lalr1.cc" /* -*- C++ -*- */
%require  "3.0"
%debug 
%defines 
%define api.namespace {whitemech::lydia}

/*
 * bison 3.3.2 deprecates %define parser_class_name {}
 * for %define api.parser.class {}, but
 * we want backward compatibility for bison 3.0.4.
 */
%define parser_class_name {Parser}

%code requires{
   #include "logic.hpp"
   #include "parser_stype.h"
   namespace whitemech {
   namespace lydia {
      class Driver;
      class Scanner;
   }
   }

// The following definitions is missing when %locations isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}

%parse-param { Scanner  &scanner  }
%parse-param { Driver  &driver  }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   
   /* include for all driver functions */
   #include "driver.hpp"

#undef yylex
#define yylex scanner.yylex
}

%define parse.assert

%define api.value.type {struct whitemech::lydia::YYSTYPE}

%type<formula> input
%type<formula> ldlf_formula

%token                  LPAR
%token                  RPAR
%token                  BOX_LPAR
%token                  BOX_RPAR
%token                  DIAMOND_LPAR
%token                  DIAMOND_RPAR
%token                  UNION
%token                  SEQUENCE
%token                  TEST
%token                  STAR
%token                  TT
%token                  FF
%token                  TRUE
%token                  FALSE
%token                  SYMBOL
%token                  NEWLINE
%token                  END_OF_FILE    0

%left                   EQUIVALENCE
%right                  IMPLICATION
%left                   OR
%left                   AND
%right                  NOT
%nonassoc               LPAR

%locations

%start input

%%

input
    : ldlf_formula { $$ = $1;
                     driver.result = $$; }
    ;

ldlf_formula
    : TT                                { $$ = driver.add_LDLfBooleanAtom(true); }
    | FF                                { $$ = driver.add_LDLfBooleanAtom(false); }
    | LPAR ldlf_formula RPAR            { $$ = $2; }
    | ldlf_formula AND ldlf_formula     { $$ = driver.add_LDLfAnd($1, $3); }
    | ldlf_formula OR ldlf_formula      { $$ = driver.add_LDLfOr($1, $3); }
    | NOT ldlf_formula                  { $$ = driver.add_LDLfNot($2); }
    | NEWLINE                           { driver.add_newline(); }
    ;

%%


void whitemech::lydia::Parser::error(const location_type &l, const std::string &err_message) {
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
