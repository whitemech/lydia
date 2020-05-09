/*
 * This file is part of Lydia.
 *
 * Lydia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Lydia is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Lydia.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "CLI/CLI.hpp"
#include <iostream>
#include <istream>
#include <lydia/parser/driver.cpp>

int main(int argc, char **argv) {
  CLI::App app{"A tool for LDLf automata translation and LDLf synthesis."};

  std::string ldlf_formula;
  CLI::Option *str_opt =
      app.add_option("-l,--ldlf", ldlf_formula, "An LDLf formula.");

  // TODO add support for reading from a file .ldlf
  //  std::string filename = "formula";
  //  CLI::Option *file_opt = app.add_option("-f,--file", filename, "A file
  //  containing the LDLf formula.");
  //
  //  file_opt->excludes(str_opt);
  //  str_opt->excludes(file_opt);

  CLI11_PARSE(app, argc, argv);

  auto driver = whitemech::lydia::Driver();

  if (str_opt) {
    std::stringstream ldlf_formula_stream(ldlf_formula);
    driver.parse(ldlf_formula_stream);
    driver.print(std::cout);
  }

  return 0;
}
