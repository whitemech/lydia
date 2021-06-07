/*
 * This file is part of Lydia.
 *
 * Lydia is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Lydia is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lydia.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "CLI/CLI.hpp"
#include <iostream>
#include <istream>
#include <lydia/dfa/mona_dfa.hpp>
#include <lydia/parser/ldlf/driver.cpp>
#include <lydia/parser/ltlf/driver.cpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>
#include <lydia/utils/print.hpp>
#include <synthesis/syn.h>

enum class Logic : int { ldlf, ltlf };

int main(int argc, char** argv) {
  whitemech::lydia::Logger logger("main");
  whitemech::lydia::Logger::level(whitemech::lydia::LogLevel::info);

  CLI::App app{
      "A tool for LTLf/LDLf automata translation and LTLf/LDLf synthesis."};

  bool no_empty = false;
  app.add_flag("-n,--no-empty", no_empty, "Enforce non-empty semantics.");
  bool quiet = false;
  app.add_flag("-q,--quiet", quiet, "Set quiet mode.");
  bool verbose = false;
  app.add_flag("-v,--verbose", verbose, "Set verbose mode.");
  bool version = false;
  app.add_flag("-V,--version", version, "Print the version and exit.");
  bool summary = false;
  app.add_flag("-s,--summary", summary, "Print the summary.");
  bool print_dfa = false;
  app.add_flag("-p,--print", print_dfa, "Print the DFA.");

  std::string graphviz_path;
  CLI::Option* dot_option =
      app.add_option("-g, --graphviz", graphviz_path,
                     "Output the automaton in Graphviz format.")
          ->check(CLI::NonexistentPath);

  // TODO add possibility to print in HOA format in future work
  //  bool hoa_flag = false;
  //  app.add_option("-a, --hoa", hoa_flag, "Output the
  //  automaton in HOA format.");

  // define logic options
  Logic logic;
  std::vector<std::pair<std::string, Logic>> map{{"ldlf", Logic::ldlf},
                                                 {"ltlf", Logic::ltlf}};
  CLI::Option* logic_opt =
      app.add_option("-l,--logic", logic, "Logic.")
          ->required()
          ->transform(CLI::CheckedTransformer(map, CLI::ignore_case));

  // options & flags
  std::string filename;
  std::string formula;

  auto format = app.add_option_group("Input format");
  CLI::Option* formula_opt = app.add_option("-i,--inline", formula, "Formula.");
  CLI::Option* file_opt =
      app.add_option("-f,--file", filename, "File to formula.")
          ->check(CLI::ExistingFile);
  formula_opt->excludes(file_opt);
  file_opt->excludes(formula_opt);
  format->add_option(formula_opt);
  format->add_option(file_opt);
  format->require_option(1, 1);
  format->needs(logic_opt);

  std::string part_file;
  bool starting_player_env = false;
  CLI::Option* part_opt = app.add_option("--part", part_file, "Part file.")
                              ->check(CLI::ExistingFile);
  app.add_flag("--env", starting_player_env, "Check env realizability.")
      ->needs(part_opt);

  CLI11_PARSE(app, argc, argv)

  if (quiet) {
    whitemech::lydia::Logger::level(whitemech::lydia::LogLevel::error);
  }

  if (verbose) {
    whitemech::lydia::Logger::level(whitemech::lydia::LogLevel::debug);
  }

  if (version) {
    std::cout << LYDIA_VERSION << std::endl;
    return 0;
  }

  // TODO make it configurable
  auto dfa_strategy = whitemech::lydia::CompositionalStrategy();
  auto translator = whitemech::lydia::Translator(dfa_strategy);

  std::shared_ptr<whitemech::lydia::AbstractDriver> driver;
  if (logic == Logic::ldlf) {
    driver = std::make_shared<whitemech::lydia::parsers::ldlf::Driver>();
  } else if (logic == Logic::ltlf) {
    driver = std::make_shared<whitemech::lydia::parsers::ltlf::LTLfDriver>();
  }

  if (!file_opt->empty()) {
    std::filesystem::path formula_path(filename);
    logger.info("Parsing {}", formula_path);
    driver->parse(formula_path.string().c_str());
  } else {
    std::stringstream formula_stream(formula);
    logger.info("Parsing {}", formula);
    driver->parse(formula_stream);
  }

  auto parsed_formula = driver->get_result();
  if (no_empty) {
    logger.info("Apply no-empty semantics.");
    auto context = driver->context;
    auto end = context->makeLdlfEnd();
    auto not_end = context->makeLdlfNot(end);
    parsed_formula = context->makeLdlfAnd({parsed_formula, not_end});
  }

  auto t_start = std::chrono::high_resolution_clock::now();

  logger.info("Transforming to DFA...");
  auto t_dfa_start = std::chrono::high_resolution_clock::now();
  auto my_dfa = translator.to_dfa(*parsed_formula);
  // TODO make 'dfa' abstraction stronger
  auto my_mona_dfa =
      std::dynamic_pointer_cast<whitemech::lydia::mona_dfa>(my_dfa);
  auto t_dfa_end = std::chrono::high_resolution_clock::now();
  logger.info("Transforming to DFA...done!");
  double elapsed_time_dfa =
      std::chrono::duration<double, std::milli>(t_dfa_end - t_dfa_start)
          .count();
  logger.info("Time elapsed for DFA construction: {}ms", elapsed_time_dfa);

  if (summary) {
    // TODO add more details
    logger.info("Number of states " +
                std::to_string(my_mona_dfa->get_nb_states()));
  }

  if (print_dfa) {
    logger.info("Computed automaton:");
    whitemech::lydia::dfaPrint(my_mona_dfa->get_dfa(),
                               my_mona_dfa->get_nb_variables(),
                               my_mona_dfa->names, my_mona_dfa->indices.data());
  }

  if (!dot_option->empty()) {
    logger.info("Printing the automaton...");
    // workaround to use MONA printer if possible
    if (my_mona_dfa) {
      whitemech::lydia::print_mona_dfa(my_mona_dfa->get_dfa(), graphviz_path,
                                       my_mona_dfa->get_nb_variables());
    } else {
      logger.error("Failed.");
    }
  }

  // synthesis part
  if (part_opt->empty()) {
    // stop here.
    double elapsed_time =
        std::chrono::duration<double, std::milli>(t_dfa_end - t_start).count();
    logger.info("Overall time elapsed: {}ms", elapsed_time);
    return 0;
  }

  Cudd* mgr = new Cudd();
  bool res = false;
  std::unordered_map<unsigned, BDD> strategy;

  // export DFA in MONA format, so it can be used by Syft
  char* temp_filename = tmpnam(nullptr);
  std::string temp_filename_string = std::string(temp_filename);
  logger.info("Writing MONA DFA into '{}'...", temp_filename_string);
  my_mona_dfa->export_dfa(temp_filename_string);

  logger.info("Start synthesis...");
  auto t_syn_start = std::chrono::high_resolution_clock::now();
  Syft::syn test(mgr, temp_filename_string, part_file);

  if (starting_player_env) {
    logger.info("Testing environment realizability...");
    res = test.realizablity_env(strategy);
  } else {
    logger.info("Testing system realizability...");
    res = test.realizablity_sys(strategy);
  }

  if (res)
    logger.info("realizable.");
  else
    logger.info("unrealizable.");

  auto t_syn_end = std::chrono::high_resolution_clock::now();
  double elapsed_time_syn =
      std::chrono::duration<double, std::milli>(t_syn_end - t_syn_start)
          .count();
  logger.info("Time elapsed for synthesis: {}ms", elapsed_time_syn);
  double elapsed_time =
      std::chrono::duration<double, std::milli>(t_syn_end - t_start).count();
  logger.info("Overall time elapsed: {}ms", elapsed_time);

  return 0;
}
