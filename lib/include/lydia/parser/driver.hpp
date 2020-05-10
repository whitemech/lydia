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

#include <cstddef>
#include <istream>
#include <string>

#include "parser.tab.hh"
#include <lydia/logic.hpp>
#include <lydia/parser/scanner.hpp>
#include <lydia/pl/logic.hpp>
#include <lydia/symbol.hpp>

namespace whitemech {
namespace lydia {

class Driver {
private:
  void parse_helper(std::istream &stream);

  Parser *parser = nullptr;
  Scanner *scanner = nullptr;

public:
  std::shared_ptr<const LDLfFormula> result;

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

  std::shared_ptr<const LDLfFormula>
  add_LDLfBooleanAtom(const bool &flag) const;
  std::shared_ptr<const LDLfFormula>
  add_LDLfAnd(std::shared_ptr<const LDLfFormula> &lhs,
              std::shared_ptr<const LDLfFormula> &rhs) const;
  std::shared_ptr<const LDLfFormula>
  add_LDLfOr(std::shared_ptr<const LDLfFormula> &lhs,
             std::shared_ptr<const LDLfFormula> &rhs) const;
  std::shared_ptr<const LDLfFormula>
  add_LDLfNot(std::shared_ptr<const LDLfFormula> &formula) const;

  std::shared_ptr<const LDLfFormula>
  add_LDLfDiamond(std::shared_ptr<const RegExp> &regex,
                  std::shared_ptr<const LDLfFormula> &formula) const;
  std::shared_ptr<const LDLfFormula>
  add_LDLfBox(std::shared_ptr<const RegExp> &regex,
              std::shared_ptr<const LDLfFormula> &formula) const;
  std::shared_ptr<const LDLfFormula>
  add_LDLfImplication(std::shared_ptr<const LDLfFormula> &lhs,
                      std::shared_ptr<const LDLfFormula> &rhs) const;
  std::shared_ptr<const LDLfFormula>
  add_LDLfEquivalence(std::shared_ptr<const LDLfFormula> &lhs,
                      std::shared_ptr<const LDLfFormula> &rhs) const;
  std::shared_ptr<const LDLfFormula> add_LDLfEnd() const;
  std::shared_ptr<const LDLfFormula> add_LDLfLast() const;

  std::shared_ptr<const RegExp> add_PropositionalRegExp(
      std::shared_ptr<const PropositionalFormula> &prop_formula) const;
  std::shared_ptr<const RegExp>
  add_TestRegExp(std::shared_ptr<const LDLfFormula> &formula) const;
  std::shared_ptr<const RegExp>
  add_StarRegExp(std::shared_ptr<const RegExp> &regex) const;
  std::shared_ptr<const RegExp>
  add_SequenceRegExp(std::shared_ptr<const RegExp> &regex_lhs,
                     std::shared_ptr<const RegExp> &regex_rhs) const;
  std::shared_ptr<const RegExp>
  add_UnionRegExp(std::shared_ptr<const RegExp> &regex_lhs,
                  std::shared_ptr<const RegExp> &regex_rhs) const;

  std::shared_ptr<const PropositionalFormula>
  add_PropositionalBooleanAtom(const bool &flag) const;
  std::shared_ptr<const PropositionalFormula>
  add_PropositionalAtom(std::string &symbol_name) const;
  std::shared_ptr<const PropositionalFormula>
  add_PropositionalAnd(std::shared_ptr<const PropositionalFormula> &lhs,
                       std::shared_ptr<const PropositionalFormula> &rhs) const;
  std::shared_ptr<const PropositionalFormula>
  add_PropositionalOr(std::shared_ptr<const PropositionalFormula> &lhs,
                      std::shared_ptr<const PropositionalFormula> &rhs) const;
  std::shared_ptr<const PropositionalFormula> add_PropositionalNot(
      std::shared_ptr<const PropositionalFormula> &prop_formula) const;
  std::shared_ptr<const PropositionalFormula> add_PropositionalImplication(
      std::shared_ptr<const PropositionalFormula> &lhs,
      std::shared_ptr<const PropositionalFormula> &rhs) const;
  std::shared_ptr<const PropositionalFormula> add_PropositionalEquivalence(
      std::shared_ptr<const PropositionalFormula> &lhs,
      std::shared_ptr<const PropositionalFormula> &rhs) const;

  std::ostream &print(std::ostream &stream) const;
};

} // namespace lydia
} // namespace whitemech
