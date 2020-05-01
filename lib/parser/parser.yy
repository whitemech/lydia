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
   #include "propositional_logic.hpp"
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
%parse-param { Driver  &d  }

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

%type<formula> input ldlf_formula
%type<regex> regular_expression
%type<prop_formula> propositional_formula
%type<symbol_name> SYMBOL

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
%left                   UNION
%left                   SEQUENCE
%left                   STAR
%left                   TEST
%left                   OR
%left                   AND
%left                   DIAMOND_LPAR
%left                   BOX_LPAR
%right                  DIAMOND_RPAR
%right                  BOX_RPAR
%right                  NOT
%nonassoc               LPAR

%locations

%start input

%%

input: ldlf_formula                                                                     { $$ = $1;
                                                                                          d.result = $$; };

ldlf_formula: ldlf_formula EQUIVALENCE ldlf_formula                                     { $$ = d.addLDLfEquivalence($1, $3); }
            | ldlf_formula IMPLICATION ldlf_formula                                     { $$ = d.addLDLfImplication($1, $3); }
            | ldlf_formula OR ldlf_formula                                              { $$ = d.add_LDLfOr($1, $3); }
            | ldlf_formula AND ldlf_formula                                             { $$ = d.add_LDLfAnd($1, $3); }
            | BOX_LPAR regular_expression BOX_RPAR ldlf_formula                         { $$ = d.addLDLfBox($2, $4); };
            | DIAMOND_LPAR regular_expression DIAMOND_RPAR ldlf_formula                 { $$ = d.addLDLfDiamond($2, $4); }
            | NOT ldlf_formula                                                          { $$ = d.add_LDLfNot($2); }
            | TT                                                                        { $$ = d.add_LDLfBooleanAtom(true); }
            | FF                                                                        { $$ = d.add_LDLfBooleanAtom(false); }
            ;

regular_expression: regular_expression UNION regular_expression                         { $$ = d.add_UnionRegExp($1, $3); }
                  | regular_expression SEQUENCE regular_expression                      { $$ = d.add_SequenceRegExp($1, $3); }
                  | regular_expression STAR                                             { $$ = d.add_StarRegExp($1); }
                  | ldlf_formula TEST                                                   { $$ = d.add_TestRegExp($1); }
                  | propositional_formula                                               { $$ = d.add_PropositionalRegExp($1); }
                  ;

propositional_formula: propositional_formula EQUIVALENCE propositional_formula          { $$ = d.add_PropositionalEquivalence($1, $3); }
                     | propositional_formula IMPLICATION propositional_formula          { $$ = d.add_PropositionalImplication($1, $3); }
                     | propositional_formula OR propositional_formula                   { $$ = d.add_PropositionalOr($1, $3); }
                     | propositional_formula AND propositional_formula                  { $$ = d.add_PropositionalAnd($1, $3); }
                     | NOT propositional_formula                                        { $$ = d.add_PropositionalNot($2); }
                     | FALSE                                                            { $$ = d.add_PropositionalBooleanAtom(false); }
                     | TRUE                                                             { $$ = d.add_PropositionalBooleanAtom(true); }
                     | SYMBOL                                                           { $$ = d.add_PropositionalAtom($1); }
                     ;

ldlf_formula: LPAR ldlf_formula RPAR                                                    { $$ = $2; };
regular_expression: LPAR regular_expression RPAR                                        { $$ = $2; };
propositional_formula: LPAR propositional_formula RPAR                                  { $$ = $2; };
    
%%


void whitemech::lydia::Parser::error(const location_type &l, const std::string &err_message) {
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
