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
#include "driver.cpp"
#include "translate.hpp"
#include "utils/dfa_transform.hpp"
#include <iostream>
#include <istream>

std::string dump_formula(const std::string &filename) {
  std::ifstream f(filename);
  std::string result;
  if (f) {
    std::ostringstream ss;
    ss << f.rdbuf(); // reading data
    result = ss.str();
  }
  f.close();
  return result;
}

int main(int argc, char **argv) {
  CLI::App app{"A tool for LDLf automata translation and LDLf synthesis."};

  std::string ldlf_formula;
  CLI::Option *str_opt =
      app.add_option("-l,--ldlf", ldlf_formula, "An LDLf formula.");

  std::string filename;
  CLI::Option *file_opt =
      app.add_option("-f,--file", filename,
                     "A .ldlf file containing an LDLf formula.")
          ->check(CLI::ExistingFile);

  // you can either enter the formula inline or within a file, not both.
  file_opt->excludes(str_opt);
  str_opt->excludes(file_opt);

  // TODO add possibility to print in DOT or HOA format in future work
  //  bool graphviz_flag = false;
  //  bool hoa_flag = false;
  //  app.add_flag("-g, --graphviz", graphviz_flag, "Output the automaton in
  //  Graphviz format."); app.add_flag("-a, --hoa", hoa_flag, "Output the
  //  automaton in HOA format.");

  CLI11_PARSE(app, argc, argv);

  auto driver = whitemech::lydia::Driver();
  auto mgr = CUDD::Cudd();

  if (!str_opt->empty()) {
    std::stringstream ldlf_formula_stream(ldlf_formula);
    driver.parse(ldlf_formula_stream);
  } else if (!file_opt->empty()) {
    std::string formula = dump_formula(filename);
    std::stringstream ldlf_formula_stream(formula);
    driver.parse(ldlf_formula_stream);
  }

  auto my_dfa = to_dfa(*driver.result, mgr);
  //  dfa_to_graphviz(*my_dfa, "cli_output_" + filename + ".svg", "svg");

  return 0;
}
