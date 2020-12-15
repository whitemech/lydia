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
#include <utility>

#include <lydia/ast/base.hpp>
#include <lydia/logic/ltlf/base.hpp>
#include <lydia/logic/symbol.hpp>

#include "lydia/parser/ltlf/parser.tab.hh"
#include <lydia/parser/ltlf/scanner.hpp>

namespace whitemech::lydia {

class LTLfDriver {
private:
  void parse_helper(std::istream &stream);

  LTLfParser *parser = nullptr;
  LTLfScanner *scanner = nullptr;
  std::shared_ptr<AstManager> context = nullptr;

public:
  std::shared_ptr<const LTLfFormula> result;

  LTLfDriver() { context = std::make_shared<AstManager>(); }

  explicit LTLfDriver(std::shared_ptr<AstManager> c) : context{std::move(c)} {}

  virtual ~LTLfDriver();

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

  std::shared_ptr<const LTLfFormula> add_LTLfTrue() const;

  std::shared_ptr<const LTLfFormula> add_LTLfFalse() const;

  std::shared_ptr<const LTLfFormula> add_LTLfAtom(std::string s) const;

  std::shared_ptr<const LTLfFormula>
  add_LTLfAnd(std::shared_ptr<const LTLfFormula> &lhs,
              std::shared_ptr<const LTLfFormula> &rhs) const;

  std::shared_ptr<const LTLfFormula>
  add_LTLfOr(std::shared_ptr<const LTLfFormula> &lhs,
             std::shared_ptr<const LTLfFormula> &rhs) const;

  std::shared_ptr<const LTLfFormula>
  add_LTLfNot(std::shared_ptr<const LTLfFormula> &formula) const;

  std::shared_ptr<const LTLfFormula>
  add_LTLfNext(std::shared_ptr<const LTLfFormula> &formula) const;

  std::shared_ptr<const LTLfFormula>
  add_LTLfWeakNext(std::shared_ptr<const LTLfFormula> &formula) const;

  std::shared_ptr<const LTLfFormula>
  add_LTLfEventually(std::shared_ptr<const LTLfFormula> &formula) const;

  std::shared_ptr<const LTLfFormula>
  add_LTLfAlways(std::shared_ptr<const LTLfFormula> &formula) const;

  std::shared_ptr<const LTLfFormula>
  add_LTLfUntil(std::shared_ptr<const LTLfFormula> &lhs,
                std::shared_ptr<const LTLfFormula> &rhs) const;

  std::shared_ptr<const LTLfFormula>
  add_LTLfRelease(std::shared_ptr<const LTLfFormula> &lhs,
                  std::shared_ptr<const LTLfFormula> &rhs) const;

  std::shared_ptr<const LTLfFormula>
  add_LTLfImplication(std::shared_ptr<const LTLfFormula> &lhs,
                      std::shared_ptr<const LTLfFormula> &rhs) const;

  std::shared_ptr<const LTLfFormula>
  add_LTLfEquivalence(std::shared_ptr<const LTLfFormula> &lhs,
                      std::shared_ptr<const LTLfFormula> &rhs) const;

  std::shared_ptr<const LTLfFormula> add_LTLfLast() const;

  std::ostream &print(std::ostream &stream) const;
};

} // namespace whitemech::lydia
