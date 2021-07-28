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
#include <catch.hpp>

#include "test/src/utils/to_dfa.hpp"
#include <iostream>
#include <lydia/logic/nnf.hpp>
#include <lydia/parser/ltlf/driver.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Translate X(a)", "[translate][ltlf][basic]") {
  std::string formula_name = "X[!](a)";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string<parsers::ltlf::LTLfDriver>(
      formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {"0"}, false));
  REQUIRE(verify(*automaton, {"0"}, false));
  REQUIRE(verify(*automaton, {"1", "1"}, true));
}

TEST_CASE("Translate a U b", "[translate][ltlf][basic]") {
  std::string formula_name = "a U b";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string<parsers::ltlf::LTLfDriver>(
      formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {"00"}, false));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"01", "00"}, false));
  REQUIRE(verify(*automaton, {"01", "10"}, true));
}

TEST_CASE("Translate a R b", "[translate][ltlf][basic]") {
  std::string formula_name = "a R b";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string<parsers::ltlf::LTLfDriver>(
      formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, true));
  REQUIRE(verify(*automaton, {"00"}, false));
  REQUIRE(verify(*automaton, {"01"}, false));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, true));
  REQUIRE(verify(*automaton, {"10", "00"}, false));
  REQUIRE(verify(*automaton, {"10", "01"}, false));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));
  REQUIRE(verify(*automaton, {"00", "00"}, false));
  REQUIRE(verify(*automaton, {"00", "01"}, false));
  REQUIRE(verify(*automaton, {"00", "10"}, false));
  REQUIRE(verify(*automaton, {"00", "11"}, false));
}

} // namespace whitemech::lydia::Test