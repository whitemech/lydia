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
#include "catch.hpp"
#include "nnf.hpp"
#include <iostream>
#include <translate.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Set of DFA states", "[translate]") {
  auto a = DFAState(set_nfa_states{});
  auto b = DFAState(set_formulas{std::make_shared<LDLfBooleanAtom>(true)});
  auto c = DFAState(set_nfa_states{});

  REQUIRE(a == c);
  REQUIRE(c == a);
  REQUIRE(!(a == b));
  REQUIRE(a < b);
}

TEST_CASE("Translate", "[translate]") {
  auto ff_and_tt = set_formulas({boolean(false), boolean(true)});
  auto not_and = LDLfNot(std::make_shared<LDLfAnd>(ff_and_tt));
  // TODO complete.
  to_dfa(not_and);
}

} // namespace whitemech::lydia::Test