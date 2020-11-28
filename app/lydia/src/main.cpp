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
#include <lydia/dfa/mona_dfa.hpp>
#include <lydia/parser/driver.cpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>
#include <lydia/utils/print.hpp>

void check_file(const std::filesystem::path &filepath) {
  if (not std::filesystem::exists(filepath)) {
    std::string message =
        fmt::format("File {} does not exist.", filepath.string());
    throw std::invalid_argument(message);
  }
  if (not std::filesystem::is_regular_file(filepath)) {
    std::string message =
        fmt::format("File {} is not a regular file.", filepath.string());
    throw std::invalid_argument(message);
  }
}

std::string dump_formula(const std::filesystem::path &filename) {
  std::ifstream f(filename.string());
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
  whitemech::lydia::Logger logger("main");
  whitemech::lydia::Logger::level(whitemech::lydia::LogLevel::info);

  CLI::App app{"A tool for LDLf automata translation and LDLf synthesis."};

  std::string formula;
  CLI::Option *str_opt =
      app.add_option("-l,--ldlf", formula, "An LDLf formula.");

  bool verbose;
  app.add_flag("--verbose", verbose, "Set verbose mode.");

  bool version;
  app.add_flag("--version", version, "Print the version and exit.");

  std::string filename;
  CLI::Option *file_opt = app.add_option(
      "-f,--file", filename, "A .ldlf file containing an LDLf formula.");

  // you can either enter the formula inline or within a file, not both.
  file_opt->excludes(str_opt);
  str_opt->excludes(file_opt);

  std::string graphviz_path;
  CLI::Option *dot_option =
      app.add_option("-g, --graphviz", graphviz_path,
                     "Output the automaton in Graphviz format.")
          ->check(CLI::NonexistentPath);

  bool summary = false;
  app.add_flag("-s", summary, "Print the summary.");

  // TODO add possibility to print in HOA format in future work
  //  bool hoa_flag = false;
  //  app.add_option("-a, --hoa", hoa_flag, "Output the
  //  automaton in HOA format.");

  CLI11_PARSE(app, argc, argv)

  if (version) {
    std::cout << LYDIA_VERSION << std::endl;
    return 0;
  }

  if (verbose) {
    whitemech::lydia::Logger::level(whitemech::lydia::LogLevel::debug);
  }

  auto driver = whitemech::lydia::Driver();
  // TODO make it configurable
  auto strategy = whitemech::lydia::CompositionalStrategy();
  auto translator = whitemech::lydia::Translator(strategy);

  std::stringstream formula_stream;
  if (!file_opt->empty()) {
    std::filesystem::path formula_path(filename);
    try {
      check_file(formula_path);
    } catch (const std::exception &e) {
      logger.error(e.what());
      return 2;
    }
    formula = dump_formula(formula_path);
  } else {
    logger.error("Please specify either -l or -f. Aborting...");
    return 2;
  }

  formula_stream = std::stringstream(formula);
  logger.info("parsing: {}", formula);
  driver.parse(formula_stream);

  logger.info("transforming to dfa...");
  auto t_start = std::chrono::high_resolution_clock::now();
  auto my_dfa = translator.to_dfa(*driver.result);
  auto t_end = std::chrono::high_resolution_clock::now();
  logger.info("transforming to dfa...done!");
  double elapsed_time_ms =
      std::chrono::duration<double, std::milli>(t_end - t_start).count();
  logger.info("Time elapsed: {}ms", elapsed_time_ms);

  if (summary) {
    // TODO add more details
    logger.info("Number of states " + std::to_string(my_dfa->get_nb_states()));
  }
  if (!dot_option->empty()) {
    logger.info("Printing the automaton...");
    // workaround to use MONA printer if possible
    auto my_mona_dfa =
        std::dynamic_pointer_cast<whitemech::lydia::mona_dfa>(my_dfa);
    if (my_mona_dfa) {
      whitemech::lydia::print_mona_dfa(my_mona_dfa->get_dfa(), graphviz_path,
                                       my_mona_dfa->get_nb_variables());
    } else {
      logger.error("Failed.");
    }
  }

  return 0;
}
