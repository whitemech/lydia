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

#include <cassert>
#include <cctype>
#include <fstream>

#include "driver.hpp"

namespace whitemech {
namespace lydia {

Driver::~Driver() {
  delete (scanner);
  scanner = nullptr;
  delete (parser);
  parser = nullptr;
}

void Driver::parse(const char *const filename) {
  assert(filename != nullptr);
  std::ifstream in_file(filename);
  if (!in_file.good()) {
    exit(EXIT_FAILURE);
  }
  parse_helper(in_file);
}

void Driver::parse(std::istream &stream) {
  if (!stream.good() && stream.eof()) {
    return;
  }
  parse_helper(stream);
}

void Driver::parse_helper(std::istream &stream) {
  delete (scanner);
  try {
    scanner = new Scanner(&stream);
  } catch (std::bad_alloc &ba) {
    std::cerr << "Failed to allocate scanner: (" << ba.what()
              << "), exiting!!\n";
    exit(EXIT_FAILURE);
  }

  delete (parser);
  try {
    parser = new Parser((*scanner) /* scanner */, (*this) /* driver */);
  } catch (std::bad_alloc &ba) {
    std::cerr << "Failed to allocate parser: (" << ba.what()
              << "), exiting!!\n";
    exit(EXIT_FAILURE);
  }
  const int accept(0);
  if (parser->parse() != accept) {
    std::cerr << "Parse failed!!\n";
  }
  return;
}

void Driver::add_upper() {
  uppercase++;
  chars++;
  words++;
}

void Driver::add_lower() {
  lowercase++;
  chars++;
  words++;
}

void Driver::add_word(const std::string &word) {
  words++;
  chars += word.length();
  for (const char &c : word) {
    if (islower(c)) {
      lowercase++;
    } else if (isupper(c)) {
      uppercase++;
    }
  }
}

void Driver::add_newline() {
  lines++;
  chars++;
}

void Driver::add_char() { chars++; }

std::ostream &Driver::print(std::ostream &stream) {
  stream << red << "Results: " << norm << "\n";
  stream << blue << "Uppercase: " << norm << uppercase << "\n";
  stream << blue << "Lowercase: " << norm << lowercase << "\n";
  stream << blue << "Lines: " << norm << lines << "\n";
  stream << blue << "Words: " << norm << words << "\n";
  stream << blue << "Characters: " << norm << chars << "\n";
  return (stream);
}

} // namespace lydia
} // namespace whitemech
