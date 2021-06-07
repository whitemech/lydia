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

#include <lydia/parser/ldlf/driver.hpp>

namespace whitemech::lydia::parsers::ldlf {

Driver::~Driver() {
  delete (scanner);
  scanner = nullptr;
  delete (parser);
  parser = nullptr;
}

void Driver::parse(const char* const filename) {
  assert(filename != nullptr);
  std::ifstream in_file(filename);
  if (!in_file.good()) {
    exit(EXIT_FAILURE);
  }
  parse_helper(in_file);
}

void Driver::parse(std::istream& stream) {
  if (!stream.good() && stream.eof()) {
    return;
  }
  parse_helper(stream);
}

void Driver::parse_helper(std::istream& stream) {
  delete (scanner);
  try {
    scanner = new LDLfScanner(&stream);
  } catch (std::bad_alloc& ba) {
    std::cerr << "Failed to allocate scanner: (" << ba.what()
              << "), exiting!\n";
    exit(EXIT_FAILURE);
  }

  delete (parser);
  try {
    parser = new LDLfParser((*scanner) /* scanner */, (*this) /* driver */);
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

std::shared_ptr<const LDLfFormula> Driver::add_LDLfTrue() const {
  return context->makeLdlfTrue();
}

std::shared_ptr<const LDLfFormula> Driver::add_LDLfFalse() const {
  return context->makeLdlfFalse();
}

std::shared_ptr<const LDLfFormula>
Driver::add_LDLfAnd(std::shared_ptr<const LDLfFormula>& lhs,
                    std::shared_ptr<const LDLfFormula>& rhs) const {
  set_formulas children = set_formulas({lhs, rhs});
  return context->makeLdlfAnd(children);
}

std::shared_ptr<const LDLfFormula>
Driver::add_LDLfOr(std::shared_ptr<const LDLfFormula>& lhs,
                   std::shared_ptr<const LDLfFormula>& rhs) const {
  set_formulas children = set_formulas({lhs, rhs});
  return context->makeLdlfOr(children);
}

std::shared_ptr<const LDLfFormula>
Driver::add_LDLfNot(std::shared_ptr<const LDLfFormula>& formula) const {
  return context->makeLdlfNot(formula);
}

std::shared_ptr<const LDLfFormula>
Driver::add_LDLfDiamond(std::shared_ptr<const RegExp>& regex,
                        std::shared_ptr<const LDLfFormula>& formula) const {
  return context->makeLdlfDiamond(regex, formula);
}

std::shared_ptr<const LDLfFormula>
Driver::add_LDLfBox(std::shared_ptr<const RegExp>& regex,
                    std::shared_ptr<const LDLfFormula>& formula) const {
  return context->makeLdlfBox(regex, formula);
}

std::shared_ptr<const LDLfFormula>
Driver::add_LDLfImplication(std::shared_ptr<const LDLfFormula>& lhs,
                            std::shared_ptr<const LDLfFormula>& rhs) const {
  // (not lhs) OR rhs
  auto ptr_not_lhs = context->makeLdlfNot(lhs);
  set_formulas children = set_formulas({ptr_not_lhs, rhs});
  return context->makeLdlfOr(children);
}

std::shared_ptr<const LDLfFormula>
Driver::add_LDLfEquivalence(std::shared_ptr<const LDLfFormula>& lhs,
                            std::shared_ptr<const LDLfFormula>& rhs) const {
  // (lhs IMPLIES rhs) AND (rhs IMPLIES lhs)
  auto ptr_left_implication = this->add_LDLfImplication(lhs, rhs);
  auto ptr_right_implication = this->add_LDLfImplication(rhs, lhs);
  set_formulas children =
      set_formulas({ptr_left_implication, ptr_right_implication});
  return context->makeLdlfAnd(children);
}

std::shared_ptr<const LDLfFormula> Driver::add_LDLfEnd() const {
  auto ptr_true = context->makePropRegex(context->makeTrue());
  auto ptr_ff = context->makeLdlfFalse();
  return this->add_LDLfBox(ptr_true, ptr_ff);
}

std::shared_ptr<const LDLfFormula> Driver::add_LDLfLast() const {
  auto ptr_true = context->makePropRegex(context->makeTrue());
  auto ptr_ff = context->makeLdlfFalse();
  auto formula = context->makeLdlfBox(ptr_true, ptr_ff);
  return this->add_LDLfDiamond(ptr_true, formula);
}

std::shared_ptr<const RegExp> Driver::add_PropositionalRegExp(
    std::shared_ptr<const PropositionalFormula>& prop_formula) const {
  return context->makePropRegex(prop_formula);
}

std::shared_ptr<const RegExp>
Driver::add_TestRegExp(std::shared_ptr<const LDLfFormula>& formula) const {
  return context->makeTestRegex(formula);
}

std::shared_ptr<const RegExp>
Driver::add_StarRegExp(std::shared_ptr<const RegExp>& regex) const {
  return context->makeStarRegex(regex);
}

std::shared_ptr<const RegExp>
Driver::add_SequenceRegExp(std::shared_ptr<const RegExp>& regex_lhs,
                           std::shared_ptr<const RegExp>& regex_rhs) const {
  vec_regex children = vec_regex({regex_lhs, regex_rhs});
  return context->makeSeqRegex(children);
}

std::shared_ptr<const RegExp>
Driver::add_UnionRegExp(std::shared_ptr<const RegExp>& regex_lhs,
                        std::shared_ptr<const RegExp>& regex_rhs) const {
  set_regex children = set_regex({regex_lhs, regex_rhs});
  return context->makeUnionRegex(children);
}

std::shared_ptr<const PropositionalFormula>
Driver::add_PropositionalBooleanAtom(const bool& flag) const {
  if (flag) {
    return context->makeTrue();
  }
  return context->makeFalse();
}

std::shared_ptr<const PropositionalFormula>
Driver::add_PropositionalAtom(std::string& symbol_name) const {
  return context->makePropAtom(symbol_name);
}

std::shared_ptr<const PropositionalFormula> Driver::add_PropositionalAnd(
    std::shared_ptr<const PropositionalFormula>& lhs,
    std::shared_ptr<const PropositionalFormula>& rhs) const {
  set_prop_formulas children = set_prop_formulas({lhs, rhs});
  return context->makePropAnd(children);
}

std::shared_ptr<const PropositionalFormula> Driver::add_PropositionalOr(
    std::shared_ptr<const PropositionalFormula>& lhs,
    std::shared_ptr<const PropositionalFormula>& rhs) const {
  set_prop_formulas children = set_prop_formulas({lhs, rhs});
  return context->makePropOr(children);
}

std::shared_ptr<const PropositionalFormula> Driver::add_PropositionalNot(
    std::shared_ptr<const PropositionalFormula>& prop_formula) const {
  return context->makePropNot(prop_formula);
}

std::shared_ptr<const PropositionalFormula>
Driver::add_PropositionalImplication(
    std::shared_ptr<const PropositionalFormula>& lhs,
    std::shared_ptr<const PropositionalFormula>& rhs) const {
  // (not lhs) OR rhs
  auto ptr_not_lhs = context->makePropNot(lhs);
  set_prop_formulas children = set_prop_formulas({ptr_not_lhs, rhs});
  return context->makePropOr(children);
}

std::shared_ptr<const PropositionalFormula>
Driver::add_PropositionalEquivalence(
    std::shared_ptr<const PropositionalFormula>& lhs,
    std::shared_ptr<const PropositionalFormula>& rhs) const {
  // (lhs IMPLIES rhs) AND (rhs IMPLIES lhs)
  auto ptr_left_implication = this->add_PropositionalImplication(lhs, rhs);
  auto ptr_right_implication = this->add_PropositionalImplication(rhs, lhs);
  set_prop_formulas children =
      set_prop_formulas({ptr_left_implication, ptr_right_implication});
  return context->makePropAnd(children);
}

std::ostream& Driver::print(std::ostream& stream) const {
  stream << this->result->str() << "\n";
  return (stream);
}

} // namespace whitemech::lydia::parsers::ldlf
