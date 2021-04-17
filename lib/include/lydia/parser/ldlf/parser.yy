%skeleton "lalr1.cc" /* -*- C++ -*- */
%require  "3.0"
%debug 
%defines
%define api.prefix {ldlf}
%define api.namespace {whitemech::lydia::parsers::ldlf}

/*
 * bison 3.3.2 deprecates %define parser_class_name {}
 * for %define api.parser.class {}, but
 * we want backward compatibility for bison 3.0.4.
 */
%define api.parser.class {LDLfParser}

%code requires{
   #include "lydia/logic/ldlf/base.hpp"
   #include "lydia/parser/ldlf/parser_stype.h"
namespace whitemech::lydia::parsers::ldlf {
      class Driver;
      class LDLfScanner;
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

%parse-param { LDLfScanner  &scanner  }
%parse-param { Driver  &d  }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   
   /* include for all driver functions */
   #include "lydia/parser/ldlf/scanner.hpp"
   #include "lydia/parser/ldlf/driver.hpp"

#undef yylex
#define yylex scanner.ldlflex
}

%define parse.assert

%define api.value.type {struct whitemech::lydia::parsers::ldlf::LDLf_YYSTYPE}

%type<formula> input ldlf_formula
%type<regex> regular_expression
%type<prop_formula> propositional_formula
%type<symbol_name> SYMBOL

%token                  LPAR
%token                  RPAR
%token                  TT
%token                  FF
%token                  END
%token                  LAST
%token                  TRUE_
%token                  FALSE_
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

ldlf_formula: ldlf_formula EQUIVALENCE ldlf_formula                                     { $$ = d.add_LDLfEquivalence($1, $3); }
            | ldlf_formula IMPLICATION ldlf_formula                                     { $$ = d.add_LDLfImplication($1, $3); }
            | ldlf_formula OR ldlf_formula                                              { $$ = d.add_LDLfOr($1, $3); }
            | ldlf_formula AND ldlf_formula                                             { $$ = d.add_LDLfAnd($1, $3); }
            | BOX_LPAR regular_expression BOX_RPAR ldlf_formula                         { $$ = d.add_LDLfBox($2, $4); };
            | DIAMOND_LPAR regular_expression DIAMOND_RPAR ldlf_formula                 { $$ = d.add_LDLfDiamond($2, $4); }
            | NOT ldlf_formula                                                          { $$ = d.add_LDLfNot($2); }
            | TT                                                                        { $$ = d.add_LDLfTrue(); }
            | FF                                                                        { $$ = d.add_LDLfFalse(); }
            | END                                                                       { $$ = d.add_LDLfEnd(); }
            | LAST                                                                      { $$ = d.add_LDLfLast(); }
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
                     | FALSE_                                                           { $$ = d.add_PropositionalBooleanAtom(false); }
                     | TRUE_                                                            { $$ = d.add_PropositionalBooleanAtom(true); }
                     | SYMBOL                                                           { $$ = d.add_PropositionalAtom($1); }
                     ;

ldlf_formula: LPAR ldlf_formula RPAR                                                    { $$ = $2; };
regular_expression: LPAR regular_expression RPAR                                        { $$ = $2; };
propositional_formula: LPAR propositional_formula RPAR                                  { $$ = $2; };
    
%%

void whitemech::lydia::parsers::ldlf::LDLfParser::error(const location_type &l, const std::string &err_message) {
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
