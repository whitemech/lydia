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

std::shared_ptr<const RegExp> Driver::add_PropositionalRegExp(
    std::shared_ptr<const PropositionalFormula> &prop_formula) const {
  return std::make_shared<PropositionalRegExp>(prop_formula);
}

std::shared_ptr<const RegExp>
Driver::add_TestRegExp(std::shared_ptr<const LDLfFormula> &formula) const {
  return std::make_shared<TestRegExp>(formula);
}

std::shared_ptr<const RegExp>
Driver::add_StarRegExp(std::shared_ptr<const RegExp> &regex) const {
  return std::make_shared<StarRegExp>(regex);
}

std::shared_ptr<const RegExp>
Driver::add_SequenceRegExp(std::shared_ptr<const RegExp> &regex_lhs,
                           std::shared_ptr<const RegExp> &regex_rhs) const {
  vec_regex children = vec_regex({regex_lhs, regex_rhs});
  return std::make_shared<SequenceRegExp>(children);
}

std::shared_ptr<const RegExp>
Driver::add_UnionRegExp(std::shared_ptr<const RegExp> &regex_lhs,
                        std::shared_ptr<const RegExp> &regex_rhs) const {
  set_regex children = set_regex({regex_lhs, regex_rhs});
  return std::make_shared<UnionRegExp>(children);
}

std::shared_ptr<const LDLfFormula>
Driver::addLDLfDiamond(std::shared_ptr<const RegExp> &regex,
                       std::shared_ptr<const LDLfFormula> &formula) const {
  return std::make_shared<LDLfDiamond>(regex, formula);
}

std::shared_ptr<const LDLfFormula>
Driver::addLDLfBox(std::shared_ptr<const RegExp> &regex,
                   std::shared_ptr<const LDLfFormula> &formula) const {
  return std::make_shared<LDLfBox>(regex, formula);
}

std::shared_ptr<const LDLfFormula>
Driver::addLDLfImplication(std::shared_ptr<const LDLfFormula> &lhs,
                           std::shared_ptr<const LDLfFormula> &rhs) const {
  // (not lhs) OR rhs
  auto ptr_not_lhs = std::make_shared<LDLfNot>(lhs);
  set_formulas children = set_formulas({ptr_not_lhs, rhs});
  return std::make_shared<LDLfOr>(children);
}

std::shared_ptr<const LDLfFormula>
Driver::addLDLfEquivalence(std::shared_ptr<const LDLfFormula> &lhs,
                           std::shared_ptr<const LDLfFormula> &rhs) const {
  // (lhs IMPLIES rhs) AND (rhs IMPLIES lhs)
  auto ptr_left_implication = this->addLDLfImplication(lhs, rhs);
  auto ptr_right_implication = this->addLDLfImplication(rhs, lhs);
  set_formulas children =
      set_formulas({ptr_left_implication, ptr_right_implication});
  return std::make_shared<LDLfAnd>(children);
}

std::shared_ptr<const PropositionalFormula>
Driver::add_PropositionalBooleanAtom(const bool &flag) const {

  if (flag) {
    return std::make_shared<PropositionalTrue>();
  }
  return std::make_shared<PropositionalFalse>();
}

std::shared_ptr<const PropositionalFormula>
Driver::add_PropositionalAtom(std::string &symbol_name) const {
  return std::make_shared<PropositionalAtom>(symbol_name);
}

std::shared_ptr<const PropositionalFormula> Driver::add_PropositionalAnd(
    std::shared_ptr<const PropositionalFormula> &lhs,
    std::shared_ptr<const PropositionalFormula> &rhs) const {
  set_prop_formulas children = set_prop_formulas({lhs, rhs});
  return std::make_shared<PropositionalAnd>(children);
}

std::shared_ptr<const PropositionalFormula> Driver::add_PropositionalOr(
    std::shared_ptr<const PropositionalFormula> &lhs,
    std::shared_ptr<const PropositionalFormula> &rhs) const {
  set_prop_formulas children = set_prop_formulas({lhs, rhs});
  return std::make_shared<PropositionalOr>(children);
}

std::shared_ptr<const PropositionalFormula> Driver::add_PropositionalNot(
    std::shared_ptr<const PropositionalFormula> &prop_formula) const {
  return std::make_shared<PropositionalNot>(prop_formula);
}

std::shared_ptr<const PropositionalFormula>
Driver::add_PropositionalImplication(
    std::shared_ptr<const PropositionalFormula> &lhs,
    std::shared_ptr<const PropositionalFormula> &rhs) const {
  // (not lhs) OR rhs
  auto ptr_not_lhs = std::make_shared<PropositionalNot>(lhs);
  set_prop_formulas children = set_prop_formulas({ptr_not_lhs, rhs});
  return std::make_shared<PropositionalOr>(children);
}

std::shared_ptr<const PropositionalFormula>
Driver::add_PropositionalEquivalence(
    std::shared_ptr<const PropositionalFormula> &lhs,
    std::shared_ptr<const PropositionalFormula> &rhs) const {
  // (lhs IMPLIES rhs) AND (rhs IMPLIES lhs)
  auto ptr_left_implication = this->add_PropositionalImplication(lhs, rhs);
  auto ptr_right_implication = this->add_PropositionalImplication(rhs, lhs);
  set_prop_formulas children =
      set_prop_formulas({ptr_left_implication, ptr_right_implication});
  return std::make_shared<PropositionalAnd>(children);

  // ((not lhs) OR rhs) AND ((not rhs) OR lhs)
  //  auto ptr_not_lhs = std::make_shared<PropositionalNot>(lhs);
  //  auto ptr_not_rhs = std::make_shared<PropositionalNot>(rhs);
  //  set_prop_formulas or_children_left = set_prop_formulas({ptr_not_lhs,
  //  rhs}); set_prop_formulas or_children_right =
  //  set_prop_formulas({ptr_not_rhs, lhs}); auto ptr_or_left =
  //  std::make_shared<PropositionalOr>(or_children_left); auto ptr_or_right =
  //  std::make_shared<PropositionalOr>(or_children_right); set_prop_formulas
  //  and_children = set_prop_formulas({ptr_or_left, ptr_or_right});

  //  return std::make_shared<PropositionalAnd>(and_children);
}

std::ostream &Driver::print(std::ostream &stream) const {
  stream << this->result->str() << "\n";
  return (stream);
}

} // namespace lydia
} // namespace whitemech
