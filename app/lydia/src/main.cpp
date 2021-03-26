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
#include <lydia/parser/ldlf/driver.cpp>
#include <lydia/parser/ltlf/driver.cpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>
#include <lydia/utils/print.hpp>
#include <synthesis/syn.h>

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
  bool verbose = false;
  app.add_flag("--verbose", verbose, "Set verbose mode.");
  bool version = false;
  app.add_flag("--version", version, "Print the version and exit.");
  bool print_dfa = false;
  app.add_flag("-p,--print", print_dfa, "Print the DFA.");

  bool no_empty = false;
  app.add_flag("-n,--no-empty", no_empty, "Enforce non-empty semantics.");

  // define --ltlf and --ldlf
  auto formula_group = app.add_option_group("formula");
  std::string ldlf_file;
  std::string ltlf_file;
  formula_group->require_option();
  CLI::Option *ldlf_file_opt =
      formula_group->add_option("--ldlf", ldlf_file, "File to an LDLf formula.")
          ->check(CLI::ExistingFile);
  CLI::Option *ltlf_file_opt =
      formula_group->add_option("--ltlf", ltlf_file, "File to an LTLf formula.")
          ->check(CLI::ExistingFile);
  // --ldlf and --ltlf are mutually exclusive.
  ldlf_file_opt->excludes(ltlf_file_opt);
  ltlf_file_opt->excludes(ldlf_file_opt);

  std::string part_file;
  CLI::Option *part_file_opt = app.add_option("--part", part_file, "Part file.")
                                   ->check(CLI::ExistingFile);

  bool starting_player_env = false;
  CLI::Option *starting_player_env_opt =
      app.add_flag("--env", starting_player_env, "Check env realizability.");
  starting_player_env_opt->needs(part_file_opt);

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

  // TODO make it configurable
  auto dfa_strategy = whitemech::lydia::CompositionalStrategy();
  auto translator = whitemech::lydia::Translator(dfa_strategy);

  std::string filename = ldlf_file_opt->empty() ? ltlf_file : ldlf_file;
  std::shared_ptr<whitemech::lydia::AbstractDriver> driver;
  if (!ldlf_file_opt->empty())
    driver = std::make_shared<whitemech::lydia::parsers::ldlf::Driver>();
  else {
    driver = std::make_shared<whitemech::lydia::parsers::ltlf::LTLfDriver>();
  }
  std::filesystem::path formula_path(filename);
  logger.info("parsing {}", formula_path);
  driver->parse(formula_path.string().c_str());

  auto parsed_formula = driver->get_result();
  if (no_empty) {
    logger.info("Apply no-empty semantics.");
    auto context = driver->context;
    auto end = context->makeLdlfEnd();
    auto not_end = context->makeLdlfNot(end);
    parsed_formula = context->makeLdlfAnd({parsed_formula, not_end});
  }

  auto t_start = std::chrono::high_resolution_clock::now();

  logger.info("transforming to DFA...");
  auto t_dfa_start = std::chrono::high_resolution_clock::now();
  auto my_dfa = translator.to_dfa(*parsed_formula);
  // TODO make 'dfa' abstraction stronger
  auto my_mona_dfa =
      std::dynamic_pointer_cast<whitemech::lydia::mona_dfa>(my_dfa);
  auto t_dfa_end = std::chrono::high_resolution_clock::now();
  logger.info("transforming to DFA...done!");
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
    dfaPrint(my_mona_dfa->dfa_, my_mona_dfa->get_nb_variables(),
             reinterpret_cast<char **>(my_mona_dfa->names.data()),
             my_mona_dfa->indices.data());
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
  if (part_file_opt->empty()) {
    // stop here.
    double elapsed_time =
        std::chrono::duration<double, std::milli>(t_dfa_end - t_start).count();
    logger.info("Overall time elapsed: {}ms", elapsed_time);
    return 0;
  }

  Cudd *mgr = new Cudd();
  bool res = false;
  std::unordered_map<unsigned, BDD> strategy;

  // export DFA in MONA format, so it can be used by Syft
  char *temp_filename = tmpnam(nullptr);
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
