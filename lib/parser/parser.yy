%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {whitemech::lydia}

/*
 * bison 3.3.2 deprecates %define parser_class_name
 * for %define api.parser.class {Parser}, but
 * we want backward compatibility for bison 3.0.4.
 */
%define parser_class_name {Parser}

%code requires{
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

%define api.value.type variant
%define parse.assert

%token               END    0     "end of file"
%token               UPPER
%token               LOWER
%token <std::string> WORD
%token               NEWLINE
%token               CHAR

%locations

%%

list_option : END | list END;

list
  : item
  | list item
  ;

item
  : UPPER   { driver.add_upper(); }
  | LOWER   { driver.add_lower(); }
  | WORD    { driver.add_word( $1 ); }
  | NEWLINE { driver.add_newline(); }
  | CHAR    { driver.add_char(); }
  ;

%%


void whitemech::lydia::Parser::error(const location_type &l, const std::string &err_message) {
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
