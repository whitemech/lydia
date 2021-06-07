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
#include <lydia/logic/symbol.hpp>

#include "lydia/parser/ltlf/parser.tab.hh"
#include <lydia/parser/abstract_driver.hpp>
#include <lydia/parser/ltlf/scanner.hpp>

namespace whitemech::lydia::parsers::ltlf {

class LTLfDriver : public AbstractDriver {
private:
  void parse_helper(std::istream& stream);

  LTLfParser* parser = nullptr;
  LTLfScanner* scanner = nullptr;

public:
  ldlf_ptr result;

  LTLfDriver() : AbstractDriver() {}

  explicit LTLfDriver(std::shared_ptr<AstManager> c) : AbstractDriver(c) {}

  virtual ~LTLfDriver();

  ldlf_ptr get_result() override { return result; }

  /**
   * parse - parse from a file
   * @param filename - valid string with input file
   */
  void parse(const char* const filename) override;

  /**
   * parse - parse from a c++ input stream
   * @param is - std::istream&, valid input stream
   */
  void parse(std::istream& iss);

  ldlf_ptr add_LTLfTrue() const;
  ldlf_ptr add_LTLfFalse() const;
  ldlf_ptr add_LTLfAtom(std::string s) const;
  ldlf_ptr add_LTLfAnd(ldlf_ptr& lhs, ldlf_ptr& rhs) const;
  ldlf_ptr add_LTLfOr(ldlf_ptr& lhs, ldlf_ptr& rhs) const;
  ldlf_ptr add_LTLfNot(ldlf_ptr& formula) const;
  ldlf_ptr add_LTLfNext(ldlf_ptr& formula) const;
  ldlf_ptr add_LTLfWeakNext(ldlf_ptr& formula) const;
  ldlf_ptr add_LTLfEventually(ldlf_ptr& formula) const;
  ldlf_ptr add_LTLfAlways(ldlf_ptr& formula) const;
  ldlf_ptr add_LTLfUntil(ldlf_ptr& lhs, ldlf_ptr& rhs) const;
  ldlf_ptr add_LTLfRelease(ldlf_ptr& lhs, ldlf_ptr& rhs) const;
  ldlf_ptr add_LTLfImplication(ldlf_ptr& lhs, ldlf_ptr& rhs) const;
  ldlf_ptr add_LTLfEquivalence(ldlf_ptr& lhs, ldlf_ptr& rhs) const;
  ldlf_ptr add_LTLfLast() const;
  ldlf_ptr add_LTLfEnd() const;
  ldlf_ptr add_LTLfPropTrue() const;
  ldlf_ptr add_LTLfPropFalse() const;

  std::ostream& print(std::ostream& stream) const;
};

} // namespace whitemech::lydia::parsers::ltlf
