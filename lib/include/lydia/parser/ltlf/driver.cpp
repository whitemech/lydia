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

ldlf_ptr LTLfDriver::add_LTLfTrue() const {
  auto prop_true = context->makePropRegex(context->makeTrue());
  auto logical_true = context->makeLdlfTrue();
  return context->makeLdlfDiamond(prop_true, logical_true);
}

ldlf_ptr LTLfDriver::add_LTLfFalse() const {
  auto prop_true = context->makePropRegex(context->makeFalse());
  auto logical_true = context->makeLdlfTrue();
  return context->makeLdlfDiamond(prop_true, logical_true);
}

ldlf_ptr LTLfDriver::add_LTLfAtom(std::string s) const {
  auto prop_atom = context->makePropRegex(context->makePropAtom(s));
  auto logical_true = context->makeLdlfTrue();
  return context->makeLdlfDiamond(prop_atom, logical_true);
}

ldlf_ptr LTLfDriver::add_LTLfAnd(ldlf_ptr& lhs, ldlf_ptr& rhs) const {
  auto children = set_formulas({lhs, rhs});
  return context->makeLdlfAnd(children);
}

ldlf_ptr LTLfDriver::add_LTLfOr(ldlf_ptr& lhs, ldlf_ptr& rhs) const {
  auto children = set_formulas({lhs, rhs});
  return context->makeLdlfOr(children);
}

ldlf_ptr LTLfDriver::add_LTLfNot(ldlf_ptr& formula) const {
  return context->makeLdlfNot(formula);
}

ldlf_ptr LTLfDriver::add_LTLfNext(ldlf_ptr& formula) const {
  auto not_end = context->makeLdlfNot(context->makeLdlfEnd());
  auto formula_and_not_end = context->makeLdlfAnd({formula, not_end});
  return context->makeLdlfDiamond(context->makePropRegex(context->makeTrue()),
                                  formula_and_not_end);
}

ldlf_ptr LTLfDriver::add_LTLfWeakNext(ldlf_ptr& formula) const {
  auto end = context->makeLdlfEnd();
  auto formula_or_end = context->makeLdlfOr({formula, end});
  return context->makeLdlfBox(context->makePropRegex(context->makeTrue()),
                              formula_or_end);
}

ldlf_ptr LTLfDriver::add_LTLfEventually(ldlf_ptr& formula) const {
  auto not_end = context->makeLdlfNot(context->makeLdlfEnd());
  auto formula_and_not_end = context->makeLdlfAnd({formula, not_end});
  auto true_star =
      context->makeStarRegex(context->makePropRegex(context->makeTrue()));
  return context->makeLdlfDiamond(true_star, formula_and_not_end);
}

ldlf_ptr LTLfDriver::add_LTLfAlways(ldlf_ptr& formula) const {
  auto end = context->makeLdlfEnd();
  auto formula_or_end = context->makeLdlfOr({formula, end});
  auto true_star =
      context->makeStarRegex(context->makePropRegex(context->makeTrue()));
  return context->makeLdlfBox(true_star, formula_or_end);
}

ldlf_ptr LTLfDriver::add_LTLfUntil(ldlf_ptr& lhs, ldlf_ptr& rhs) const {
  auto not_end = context->makeLdlfNot(context->makeLdlfEnd());
  auto formula_and_not_end = context->makeLdlfAnd({rhs, not_end});
  auto true_regex = context->makePropRegex(context->makeTrue());
  auto formula_test = context->makeTestRegex(lhs);
  auto seq_star =
      context->makeStarRegex(context->makeSeqRegex({formula_test, true_regex}));
  return context->makeLdlfDiamond(seq_star, formula_and_not_end);
}

ldlf_ptr LTLfDriver::add_LTLfRelease(ldlf_ptr& lhs, ldlf_ptr& rhs) const {
  auto end = context->makeLdlfEnd();
  auto formula_or_end = context->makeLdlfOr({rhs, end});
  auto true_regex = context->makePropRegex(context->makeTrue());
  auto formula_test = (context->makeTestRegex(context->makeLdlfNot(lhs)));
  auto seq_star =
      context->makeStarRegex(context->makeSeqRegex({formula_test, true_regex}));
  return context->makeLdlfBox(seq_star, formula_or_end);
}

ldlf_ptr LTLfDriver::add_LTLfImplication(ldlf_ptr& lhs, ldlf_ptr& rhs) const {
  auto ptr_not_lhs = context->makeLdlfNot(lhs);
  auto children = set_formulas({ptr_not_lhs, rhs});
  return context->makeLdlfOr(children);
}

ldlf_ptr LTLfDriver::add_LTLfEquivalence(ldlf_ptr& lhs, ldlf_ptr& rhs) const {
  auto ptr_left_implication = this->add_LTLfImplication(lhs, rhs);
  auto ptr_right_implication = this->add_LTLfImplication(rhs, lhs);
  auto children = set_formulas({ptr_left_implication, ptr_right_implication});
  return context->makeLdlfAnd(children);
}

ldlf_ptr LTLfDriver::add_LTLfLast() const {
  return context->makeLdlfDiamond(context->makePropRegex(context->makeTrue()),
                                  context->makeLdlfEnd());
}

ldlf_ptr LTLfDriver::add_LTLfEnd() const { return context->makeLdlfEnd(); }

ldlf_ptr LTLfDriver::add_LTLfPropTrue() const {
  return context->makeLdlfDiamond(context->makePropRegex(context->makeTrue()),
                                  context->makeLdlfTrue());
}

ldlf_ptr LTLfDriver::add_LTLfPropFalse() const {
  return context->makeLdlfDiamond(context->makePropRegex(context->makeFalse()),
                                  context->makeLdlfTrue());
}

std::ostream& LTLfDriver::print(std::ostream& stream) const {
  stream << this->result->str() << "\n";
  return (stream);
}

} // namespace whitemech::lydia::parsers::ltlf
