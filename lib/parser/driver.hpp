#pragma once
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

#ifndef __MCDRIVER_HPP__
#define __MCDRIVER_HPP__ 1

#include <cstddef>
#include <istream>
#include <string>

#include "parser.tab.hh"
#include "scanner.hpp"

namespace whitemech {
namespace lydia {

class Driver {
public:
  Driver() = default;

  virtual ~Driver();

  /**
   * parse - parse from a file
   * @param filename - valid string with input file
   */
  void parse(const char *const filename);
  /**
   * parse - parse from a c++ input stream
   * @param is - std::istream&, valid input stream
   */
  void parse(std::istream &iss);

  void add_upper();
  void add_lower();
  void add_word(const std::string &word);
  void add_newline();
  void add_char();

  std::ostream &print(std::ostream &stream);

private:
  void parse_helper(std::istream &stream);

  std::size_t chars = 0;
  std::size_t words = 0;
  std::size_t lines = 0;
  std::size_t uppercase = 0;
  std::size_t lowercase = 0;
  Parser *parser = nullptr;
  Scanner *scanner = nullptr;

  const std::string red = "\033[1;31m";
  const std::string blue = "\033[1;36m";
  const std::string norm = "\033[0m";
};

} // namespace lydia
} // namespace whitemech
#endif /* END __DRIVER_HPP__ */
