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

/*
 * This command-line tool will translate a MONA DFA file
 * (see Appendix C.1 of https://www.brics.dk/mona/mona14.pdf) into
 * a Symbolic DFA (see https://arxiv.org/pdf/1705.08426.pdf for reference).
 *
 * In particular, it will print in DOT format the BDDs for each bit state.
 * in a user-specified output directory.
 */

#include "CLI/CLI.hpp"
#include <dfa.hpp>
#include <filesystem>

/*!
 * From MONA DFA file to Symbolic DFA representation.
 *
 * It parses the MONA DFA file and prints the BDD in the output directory.
 *
 * @param filename the path to the MONA DFA file.
 * @return nothing.
 */
void transform(const std::filesystem::path &input,
               const std::filesystem::path &output_dir) {
  auto my_dfa = whitemech::lydia::dfa::read_from_file(input);
  std::filesystem::create_directory(output_dir);
  my_dfa->bdd2dot(output_dir);
}

int main(int argc, char **argv) {
  CLI::App app{"Translate a MONA DFA into a Symbolic DFA.", "mona2sym"};

  std::string input_file;
  app.add_option("-i,--input,input_file", input_file,
                 "Path to the MONA DFA file.")
      ->check(CLI::ExistingFile);

  std::string output_directory;
  app.add_option("-o,--output-dir,output_directory", output_directory,
                 "Path to the output directory.")
      ->check(CLI::NonexistentPath);

  CLI11_PARSE(app, argc, argv);

  std::filesystem::path input_path = input_file;
  std::filesystem::path output_directory_path = output_directory;
  transform(input_path, output_directory_path);

  return 0;
}