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

#include <cassert>
#include <fstream>

#include <lydia/parser/ltlf/driver.hpp>

namespace whitemech::lydia::parsers::ltlf {

LTLfDriver::~LTLfDriver() {
  delete (scanner);
  scanner = nullptr;
  delete (parser);
  parser = nullptr;
}

void LTLfDriver::parse(const char* const filename) {
  assert(filename != nullptr);
  std::ifstream in_file(filename);
  if (!in_file.good()) {
    exit(EXIT_FAILURE);
  }
  parse_helper(in_file);
}

void LTLfDriver::parse(std::istream& stream) {
  if (!stream.good() && stream.eof()) {
    return;
  }
  parse_helper(stream);
}

void LTLfDriver::parse_helper(std::istream& stream) {
  delete (scanner);
  try {
    scanner = new LTLfScanner(&stream);
  } catch (std::bad_alloc& ba) {
    std::cerr << "Failed to allocate scanner: (" << ba.what()
              << "), exiting!\n";
    exit(EXIT_FAILURE);
  }

  delete (parser);
  try {
    parser = new LTLfParser((*scanner) /* scanner */, (*this) /* driver */);
  } catch (std::bad_alloc& ba) {
    std::cerr << "Failed to allocate parser: (" << ba.what() << "), exiting!\n";
    exit(EXIT_FAILURE);
  }
  const int accept(0);
  if (parser->parse() != accept) {
    std::cerr << "Parse failed!\n";
    exit(EXIT_FAILURE);
  }
}

ltlf_ptr LTLfDriver::add_LTLfTrue() const {
  return context->makeLtlfTrue();
}

ltlf_ptr LTLfDriver::add_LTLfFalse() const {
  return context->makeLtlfFalse();
}

ltlf_ptr LTLfDriver::add_LTLfAtom(std::string s) const {
  return context->makeLtlfAtom(s);
}

ltlf_ptr LTLfDriver::add_LTLfAnd(ltlf_ptr& lhs, ltlf_ptr& rhs) const {
  return context->makeLtlfAnd({lhs, rhs});
}

ltlf_ptr LTLfDriver::add_LTLfOr(ltlf_ptr& lhs, ltlf_ptr& rhs) const {
  return context->makeLtlfOr({lhs, rhs});
}

ltlf_ptr LTLfDriver::add_LTLfNot(ltlf_ptr& formula) const {
  return context->makeLtlfNot(formula);
}

ltlf_ptr LTLfDriver::add_LTLfNext(ltlf_ptr& formula) const {
  return context->makeLtlfNext(formula);
}

ltlf_ptr LTLfDriver::add_LTLfWeakNext(ltlf_ptr& formula) const {
  return context->makeLtlfWeakNext(formula);
}

ltlf_ptr LTLfDriver::add_LTLfEventually(ltlf_ptr& formula) const {
  return context->makeLtlfEventually(formula);
}

ltlf_ptr LTLfDriver::add_LTLfAlways(ltlf_ptr& formula) const {
  return context->makeLtlfAlways(formula);
}

ltlf_ptr LTLfDriver::add_LTLfUntil(ltlf_ptr& lhs, ltlf_ptr& rhs) const {
  return context->makeLtlfUntil(lhs, rhs);
}

ltlf_ptr LTLfDriver::add_LTLfRelease(ltlf_ptr& lhs, ltlf_ptr& rhs) const {
  return context->makeLtlfRelease(lhs, rhs);
}

ltlf_ptr LTLfDriver::add_LTLfImplication(ltlf_ptr& lhs, ltlf_ptr& rhs) const {
  return context->makeLtlfOr({context->makeLtlfNot(lhs), rhs});
}

ltlf_ptr LTLfDriver::add_LTLfEquivalence(ltlf_ptr& lhs, ltlf_ptr& rhs) const {
  auto both_true = context->makeLtlfAnd({lhs, rhs});
  auto both_false = context->makeLtlfAnd({context->makeLtlfNot(lhs), context->makeLtlfNot(rhs)});
  return context->makeLtlfOr({both_true, both_false});
}

ltlf_ptr LTLfDriver::add_LTLfLast() const {
  return context->makeLtlfLast();
}

std::ostream& LTLfDriver::print(std::ostream& stream) const {
  stream << this->result->str() << "\n";
  return (stream);
}

} // namespace whitemech::lydia::parsers::ltlf
