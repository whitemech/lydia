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

int main(int argc, char **argv) {
  CLI::App app{"A tool for LDLf automata translation and LDLf synthesis."};

  std::string filename = "default";
  app.add_option("-f,--file", filename, "A help string");

  CLI11_PARSE(app, argc, argv);
  return 0;
}
