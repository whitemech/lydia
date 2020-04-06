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

#include "lyra/lyra.hpp"
#include <iostream>

auto main(int argc, const char *argv[]) -> int {
  // Where we read in the argument values:
  std::string filepath;
  bool show_help = true;

  // The parser with the multiple option arguments. They are composed
  // together by the "|" operator.
  auto cli = lyra::help(show_help) |
             lyra::opt(filepath,
                       "filename")["-f"]["--file"]("Path to the Lydia file.");

  // Parse the program arguments:
  auto result = cli.parse({argc, argv});

  // Check that the arguments were valid:
  if (!result) {
    std::cerr << "Error in command line: " << result.errorMessage()
              << std::endl;
    std::cerr << cli << "\n";
    return 1;
  }

  // Show the help when asked for.
  if (show_help) {
    std::cout << cli << "\n";
    return 0;
  }

  std::cout << "filepath = " << filepath << "\n";
  return 0;
}
