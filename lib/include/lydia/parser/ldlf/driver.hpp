#pragma once
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

#include <cstddef>
#include <istream>
#include <string>
#include <utility>

#include <lydia/ast/base.hpp>
#include <lydia/logic/ldlf/base.hpp>
#include <lydia/logic/pl/base.hpp>
#include <lydia/logic/symbol.hpp>

#include "lydia/parser/ldlf/parser.tab.hh"
#include <lydia/parser/abstract_driver.hpp>
#include <lydia/parser/ldlf/scanner.hpp>

namespace whitemech::lydia::parsers::ldlf {

class Driver : public AbstractDriver {
private:
  void parse_helper(std::istream& stream);

  LDLfParser* parser = nullptr;
  LDLfScanner* scanner = nullptr;

public:
  ldlf_ptr result;

  Driver() : AbstractDriver() {}

  explicit Driver(std::shared_ptr<AstManager> c) : AbstractDriver(c) {}

  virtual ~Driver();

  ldlf_ptr get_result() override { return result; }

  /**
   * parse - parse from a file
   * @param filename - valid string with input file
   */
  void parse(const char* const filename) override;

  /**
   * parse - parse from a c++ input stream
   * @param iss - std::istream&, valid input stream
   */
  void parse(std::istream& iss);

  ldlf_ptr add_LDLfTrue() const;

  ldlf_ptr add_LDLfFalse() const;

  ldlf_ptr add_LDLfAnd(ldlf_ptr& lhs, ldlf_ptr& rhs) const;

  ldlf_ptr add_LDLfOr(ldlf_ptr& lhs, ldlf_ptr& rhs) const;

  ldlf_ptr add_LDLfNot(ldlf_ptr& formula) const;

  ldlf_ptr add_LDLfDiamond(regex_ptr& regex, ldlf_ptr& formula) const;

  ldlf_ptr add_LDLfBox(regex_ptr& regex, ldlf_ptr& formula) const;

  ldlf_ptr add_LDLfImplication(ldlf_ptr& lhs, ldlf_ptr& rhs) const;

  ldlf_ptr add_LDLfEquivalence(ldlf_ptr& lhs, ldlf_ptr& rhs) const;

  ldlf_ptr add_LDLfEnd() const;

  ldlf_ptr add_LDLfLast() const;

  regex_ptr add_PropositionalRegExp(prop_ptr& prop_formula) const;

  regex_ptr add_TestRegExp(ldlf_ptr& formula) const;

  regex_ptr add_StarRegExp(regex_ptr& regex) const;

  regex_ptr add_SequenceRegExp(regex_ptr& regex_lhs,
                               regex_ptr& regex_rhs) const;

  regex_ptr add_UnionRegExp(regex_ptr& regex_lhs, regex_ptr& regex_rhs) const;

  prop_ptr add_PropositionalBooleanAtom(const bool& flag) const;

  prop_ptr add_PropositionalAtom(std::string& symbol_name) const;

  prop_ptr add_PropositionalAnd(prop_ptr& lhs, prop_ptr& rhs) const;

  prop_ptr add_PropositionalOr(prop_ptr& lhs, prop_ptr& rhs) const;

  prop_ptr add_PropositionalNot(prop_ptr& prop_formula) const;

  prop_ptr add_PropositionalImplication(prop_ptr& lhs, prop_ptr& rhs) const;

  prop_ptr add_PropositionalEquivalence(prop_ptr& lhs, prop_ptr& rhs) const;

  std::ostream& print(std::ostream& stream) const;
};

} // namespace whitemech::lydia::parsers::ldlf
