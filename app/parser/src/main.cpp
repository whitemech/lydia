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

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "driver.hpp"

int main(const int argc, const char **argv) {
  /** check for the right # of arguments **/
  if (argc == 2) {
    whitemech::lydia::Driver driver;
    /** example for piping input from terminal, i.e., using cat **/
    if (std::strncmp(argv[1], "-o", 2) == 0) {
      driver.parse(std::cin);
    }
    /** simple help menu **/
    else if (std::strncmp(argv[1], "-h", 2) == 0) {
      std::cout << "use -o for pipe to std::cin\n";
      std::cout << "just give a filename to count from a file\n";
      std::cout << "use -h to get this menu\n";
      return (EXIT_SUCCESS);
    }
    /** example reading input from a file **/
    else {
      /** assume file, prod code, use stat to check **/
      driver.parse(argv[1]);
    }
    driver.print(std::cout) << "\n";
  } else {
    /** exit with failure condition **/
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}
