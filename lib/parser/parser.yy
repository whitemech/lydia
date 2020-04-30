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

%type<formula> ldlf_formula ldlf_equivalence ldlf_implication ldlf_or ldlf_and ldlf_not
%type<formula> ldlf_unaryop ldlf_box ldlf_diamond ldlf_wrapped ldlf_atom ldlf_tt ldlf_ff

%type<regex> regular_expression re_union re_sequence re_star re_test re_wrapped re_propositional

%type<prop_formula> propositional_formula prop_equivalence prop_implication prop_or prop_and prop_not
%type<prop_formula> prop_wrapped prop_atom prop_true prop_false atom

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
%left                   OR
%left                   AND
%right                  NOT
%nonassoc               LPAR

%locations

%start ldlf_formula

%%

ldlf_formula: ldlf_equivalence                                          { $$ = $1;
                                                                          d.result = $$; };
ldlf_equivalence
    : ldlf_implication EQUIVALENCE ldlf_implication                     { $$ = d.addLDLfEquivalence($1, $3); }
    | ldlf_implication                                                  { $$ = $1; }
    ;
ldlf_implication
    : ldlf_or IMPLICATION ldlf_or                                       { $$ = d.addLDLfImplication($1, $3); }
    | ldlf_or                                                           { $$ = $1; }
    ;
ldlf_or
    : ldlf_and OR ldlf_and                                              { $$ = d.add_LDLfOr($1, $3); }
    | ldlf_and                                                          { $$ = $1; }
    ;
ldlf_and
    : ldlf_unaryop AND ldlf_unaryop                                     { $$ = d.add_LDLfAnd($1, $3); }
    | ldlf_unaryop                                                      { $$ = $1; }
    ;
ldlf_unaryop
    : ldlf_box                                                          { $$ = $1; }
    | ldlf_diamond                                                      { $$ = $1; }
    | ldlf_not                                                          { $$ = $1; }
    | ldlf_wrapped                                                      { $$ = $1; }
    ;
ldlf_box: BOX_LPAR regular_expression BOX_RPAR ldlf_unaryop             { $$ = d.addLDLfBox($2, $4); };
ldlf_diamond
    : DIAMOND_LPAR regular_expression DIAMOND_RPAR ldlf_unaryop         { $$ = d.addLDLfDiamond($2, $4); }
    ;
ldlf_not: NOT ldlf_unaryop                                              { $$ = d.add_LDLfNot($2); };
ldlf_wrapped
    : ldlf_atom                                                         { $$ = $1; }
    | LPAR ldlf_formula RPAR                                            { $$ = $2; }
    ;
ldlf_atom
    : ldlf_tt                                                           { $$ = $1; }
    | ldlf_ff                                                           { $$ = $1; }
    ;
ldlf_tt: TT                                                             { $$ = d.add_LDLfBooleanAtom(true); };
ldlf_ff: FF                                                             { $$ = d.add_LDLfBooleanAtom(false); };

regular_expression: re_union                                            { $$ = $1; };
re_union
    : re_sequence UNION re_sequence                                     { $$ = d.add_UnionRegExp($1, $3); }
    | re_sequence                                                       { $$ = $1; }
    ;
re_sequence
    : re_star SEQUENCE re_star                                          { $$ = d.add_SequenceRegExp($1, $3); }
    | re_star                                                           { $$ = $1; }
    ;
re_star
    : re_test STAR                                                      { $$ = d.add_StarRegExp($1); }
    | re_test                                                           { $$ = $1; }
    ;
re_test
    : TEST ldlf_formula                                                 { $$ = d.add_TestRegExp($2); }
    | re_wrapped                                                        { $$ = $1; }
    ;
re_wrapped
    : re_propositional                                                  { $$ = $1; }
    | LPAR regular_expression RPAR                                      { $$ = $2; }
    ;
re_propositional: propositional_formula                                 { $$ = d.add_PropositionalRegExp($1); };

propositional_formula: prop_equivalence                                 { $$ = $1; };
prop_equivalence
    : prop_implication EQUIVALENCE prop_implication                     { $$ = d.add_PropositionalEquivalence($1, $3); }
    | prop_implication                                                  { $$ = $1; }
    ;
prop_implication
    : prop_or IMPLICATION prop_or                                       { $$ = d.add_PropositionalImplication($1, $3); }
    | prop_or                                                           { $$ = $1; }
    ;
prop_or
    : prop_and OR prop_and                                              { $$ = d.add_PropositionalOr($1, $3); }
    | prop_and                                                          { $$ = $1; }
    ;
prop_and
    : prop_not AND prop_not                                             { $$ = d.add_PropositionalAnd($1, $3); }
    | prop_not                                                          { $$ = $1; }
    ;
prop_not
    : NOT prop_wrapped                                                  { $$ = d.add_PropositionalNot($2); }
    | prop_wrapped                                                      { $$ = $1; }
    ;
prop_wrapped
    : prop_atom                                                         { $$ = $1; }
    | LPAR propositional_formula RPAR                                   { $$ = $2; }
    ;
prop_atom
    : atom                                                              { $$ = $1; }
    | prop_true                                                         { $$ = $1; }
    | prop_false                                                        { $$ = $1; }
    ;

atom: SYMBOL                                                            { $$ = d.add_PropositionalAtom($1); };

prop_true: TRUE                                                         { $$ = d.add_PropositionalBooleanAtom(true); };

prop_false: FALSE                                                       { $$ = d.add_PropositionalBooleanAtom(false); };
    
%%


void whitemech::lydia::Parser::error(const location_type &l, const std::string &err_message) {
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
