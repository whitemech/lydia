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
#include <fstream>

#include <lydia/parser/ltlf/driver.hpp>

namespace whitemech::lydia {

LTLfDriver::~LTLfDriver() {
  delete (scanner);
  scanner = nullptr;
  delete (parser);
  parser = nullptr;
}

void LTLfDriver::parse(const char *const filename) {
  assert(filename != nullptr);
  std::ifstream in_file(filename);
  if (!in_file.good()) {
    exit(EXIT_FAILURE);
  }
  parse_helper(in_file);
}

void LTLfDriver::parse(std::istream &stream) {
  if (!stream.good() && stream.eof()) {
    return;
  }
  parse_helper(stream);
}

void LTLfDriver::parse_helper(std::istream &stream) {
  delete (scanner);
  try {
    scanner = new LTLfScanner(&stream);
  } catch (std::bad_alloc &ba) {
    std::cerr << "Failed to allocate scanner: (" << ba.what()
              << "), exiting!!\n";
    exit(EXIT_FAILURE);
  }

  delete (parser);
  try {
    parser = new LTLfParser((*scanner) /* scanner */, (*this) /* driver */);
  } catch (std::bad_alloc &ba) {
    std::cerr << "Failed to allocate parser: (" << ba.what() << "), exiting!\n";
    exit(EXIT_FAILURE);
  }
  const int accept(0);
  if (parser->parse() != accept) {
    std::cerr << "Parse failed!\n";
  }
}

std::shared_ptr<const LTLfFormula> LTLfDriver::add_LTLfTrue() const {
  return context->makeLtlfTrue();
}

std::shared_ptr<const LTLfFormula> LTLfDriver::add_LTLfFalse() const {
  return context->makeLtlfFalse();
}

std::shared_ptr<const LTLfFormula>
LTLfDriver::add_LTLfAtom(std::string s) const {
  return context->makeLtlfAtom(s);
}

std::shared_ptr<const LTLfFormula>
LTLfDriver::add_LTLfAnd(std::shared_ptr<const LTLfFormula> &lhs,
                        std::shared_ptr<const LTLfFormula> &rhs) const {
  auto children = set_ltlf_formulas({lhs, rhs});
  return context->makeLtlfAnd(children);
}

std::shared_ptr<const LTLfFormula>
LTLfDriver::add_LTLfOr(std::shared_ptr<const LTLfFormula> &lhs,
                       std::shared_ptr<const LTLfFormula> &rhs) const {
  auto children = set_ltlf_formulas({lhs, rhs});
  return context->makeLtlfOr(children);
}

std::shared_ptr<const LTLfFormula>
LTLfDriver::add_LTLfNot(std::shared_ptr<const LTLfFormula> &formula) const {
  return context->makeLtlfNot(formula);
}

std::shared_ptr<const LTLfFormula>
LTLfDriver::add_LTLfNext(std::shared_ptr<const LTLfFormula> &formula) const {
  return context->makeLtlfNext(formula);
}

std::shared_ptr<const LTLfFormula> LTLfDriver::add_LTLfWeakNext(
    std::shared_ptr<const LTLfFormula> &formula) const {
  return context->makeLtlfWeakNext(formula);
}

std::shared_ptr<const LTLfFormula> LTLfDriver::add_LTLfEventually(
    std::shared_ptr<const LTLfFormula> &formula) const {
  return context->makeLtlfEventually(formula);
}

std::shared_ptr<const LTLfFormula>
LTLfDriver::add_LTLfAlways(std::shared_ptr<const LTLfFormula> &formula) const {
  return context->makeLtlfAlways(formula);
}

std::shared_ptr<const LTLfFormula>
LTLfDriver::add_LTLfUntil(std::shared_ptr<const LTLfFormula> &lhs,
                          std::shared_ptr<const LTLfFormula> &rhs) const {
  return context->makeLtlfUntil(lhs, rhs);
}

std::shared_ptr<const LTLfFormula>
LTLfDriver::add_LTLfRelease(std::shared_ptr<const LTLfFormula> &lhs,
                            std::shared_ptr<const LTLfFormula> &rhs) const {
  return context->makeLtlfRelease(lhs, rhs);
}

std::shared_ptr<const LTLfFormula>
LTLfDriver::add_LTLfImplication(std::shared_ptr<const LTLfFormula> &lhs,
                                std::shared_ptr<const LTLfFormula> &rhs) const {
  auto ptr_not_lhs = context->makeLtlfNot(lhs);
  auto children = set_ltlf_formulas({ptr_not_lhs, rhs});
  return context->makeLtlfOr(children);
}

std::shared_ptr<const LTLfFormula>
LTLfDriver::add_LTLfEquivalence(std::shared_ptr<const LTLfFormula> &lhs,
                                std::shared_ptr<const LTLfFormula> &rhs) const {
  auto ptr_left_implication = this->add_LTLfImplication(lhs, rhs);
  auto ptr_right_implication = this->add_LTLfImplication(rhs, lhs);
  auto children =
      set_ltlf_formulas({ptr_left_implication, ptr_right_implication});
  return context->makeLtlfAnd(children);
}

std::shared_ptr<const LTLfFormula> LTLfDriver::add_LTLfLast() const {
  return context->makeLtlfWeakNext(context->makeLtlfFalse());
}

std::ostream &LTLfDriver::print(std::ostream &stream) const {
  stream << this->result->str() << "\n";
  return (stream);
}

} // namespace whitemech::lydia
