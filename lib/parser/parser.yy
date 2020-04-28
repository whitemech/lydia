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

%type<formula> item

%left                   OR
%left                   AND
%right                  NOT

%token                  END    0     "end of file"
%token                  TT
%token                  FF
%token                  NEWLINE

%locations

%%

list_option : END | list END;

list
  : item
  | list item
  ;

item
  : TT      { $$ = driver.add_LDLfBooleanAtom(true); }
  | FF      { driver.add_LDLfBooleanAtom(false); }
  | NEWLINE { driver.add_newline(); }
  ;

%%


void whitemech::lydia::Parser::error(const location_type &l, const std::string &err_message) {
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
