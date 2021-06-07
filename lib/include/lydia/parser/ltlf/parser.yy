%skeleton "lalr1.cc" /* -*- C++ -*- */
%require  "3.0"
%debug 
%defines 
%define api.prefix {ltlf}
%define api.namespace {whitemech::lydia::parsers::ltlf}

/*
 * bison 3.3.2 deprecates %define parser_class_name {}
 * for %define api.parser.class {}, but
 * we want backward compatibility for bison 3.0.4.
 */
%define api.parser.class {LTLfParser}

%code requires{
   #include "lydia/logic/ltlf/base.hpp"
   #include "lydia/parser/ltlf/parser_stype.h"
namespace whitemech::lydia::parsers::ltlf {
      class LTLfDriver;
      class LTLfScanner;
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

%parse-param { LTLfScanner  &scanner  }
%parse-param { LTLfDriver  &d  }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   
   /* include for all driver functions */
   #include "lydia/parser/ltlf/scanner.hpp"
   #include "lydia/parser/ltlf/driver.hpp"

#undef yylex
#define yylex scanner.ltlflex
}

%define parse.assert

%define api.value.type {struct whitemech::lydia::parsers::ltlf::LTLf_YYSTYPE}

%type<formula> input ltlf_formula
%type<symbol_name> SYMBOL

%token                  LPAR
%token                  RPAR
%token                  TRUE_
%token                  FALSE_
%token                  TT
%token                  FF
%token                  END
%token                  LAST
%token                  SYMBOL
%token                  NEWLINE
%token                  END_OF_FILE    0

%left                   EQUIVALENCE
%right                  IMPLICATION
%left                   UNTIL
%left                   RELEASE
%left                   OR
%left                   AND
%right                  EVENTUALLY
%right                  ALWAYS
%right                  NEXT
%right                  WEAK_NEXT
%right                  NOT
%nonassoc               LPAR

%locations

%start input

%%

input: ltlf_formula                                                                     { $$ = $1;
                                                                                          d.result = $$; };

ltlf_formula: ltlf_formula EQUIVALENCE ltlf_formula                                     { $$ = d.add_LTLfEquivalence($1, $3); }
            | ltlf_formula IMPLICATION ltlf_formula                                     { $$ = d.add_LTLfImplication($1, $3); }
            | ltlf_formula RELEASE ltlf_formula                                         { $$ = d.add_LTLfRelease($1, $3); }
            | ltlf_formula UNTIL ltlf_formula                                           { $$ = d.add_LTLfUntil($1, $3); }
            | ltlf_formula OR ltlf_formula                                              { $$ = d.add_LTLfOr($1, $3); }
            | ltlf_formula AND ltlf_formula                                             { $$ = d.add_LTLfAnd($1, $3); }
            | ALWAYS ltlf_formula                                                       { $$ = d.add_LTLfAlways($2); }
            | EVENTUALLY ltlf_formula                                                   { $$ = d.add_LTLfEventually($2); }
            | WEAK_NEXT ltlf_formula                                                    { $$ = d.add_LTLfWeakNext($2); }
            | NEXT ltlf_formula                                                         { $$ = d.add_LTLfNext($2); }
            | NOT ltlf_formula                                                          { $$ = d.add_LTLfNot($2); }
            | TT                                                                        { $$ = d.add_LTLfTrue(); }
            | FF                                                                        { $$ = d.add_LTLfFalse(); }
            | END                                                                       { $$ = d.add_LTLfEnd(); }
            | LAST                                                                      { $$ = d.add_LTLfLast(); }
            | FALSE_                                                                    { $$ = d.add_LTLfPropFalse(); }
            | TRUE_                                                                     { $$ = d.add_LTLfPropTrue(); }
            | SYMBOL                                                                    { $$ = d.add_LTLfAtom($1); }
            ;

ltlf_formula: LPAR ltlf_formula RPAR                                                    { $$ = $2; };

%%

void whitemech::lydia::parsers::ltlf::LTLfParser::error(const location_type &l, const std::string &err_message) {
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
