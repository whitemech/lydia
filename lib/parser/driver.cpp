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
}

void Driver::add_newline() { lines++; }

std::shared_ptr<const LDLfFormula>
Driver::add_LDLfBooleanAtom(const bool &flag) const {
  return std::make_shared<LDLfBooleanAtom>(flag);
}

std::shared_ptr<const LDLfFormula>
Driver::add_LDLfAnd(std::shared_ptr<const LDLfFormula> &lhs,
                    std::shared_ptr<const LDLfFormula> &rhs) const {
  set_formulas children = set_formulas({lhs, rhs});
  return std::make_shared<LDLfAnd>(children);
}

std::shared_ptr<const LDLfFormula>
Driver::add_LDLfOr(std::shared_ptr<const LDLfFormula> &lhs,
                   std::shared_ptr<const LDLfFormula> &rhs) const {
  set_formulas children = set_formulas({lhs, rhs});
  return std::make_shared<LDLfOr>(children);
}

std::shared_ptr<const LDLfFormula>
Driver::add_LDLfNot(std::shared_ptr<const LDLfFormula> &formula) const {
  return std::make_shared<LDLfNot>(formula);
}

std::ostream &Driver::print(std::ostream &stream) {
  stream << red << "Results: " << norm << "\n";
  return (stream);
}

} // namespace lydia
} // namespace whitemech
