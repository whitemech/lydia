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
#include <set>
#include <utils/dfa_transform.hpp>
#include <utils/misc.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("powerset", "[powerset]") {
  auto s = std::set<int>({0, 1, 2});
  std::vector<std::set<int>> result = powerset<int>(s);
  REQUIRE(result[0] == std::set<int>({}));
  REQUIRE(result[1] == std::set<int>({0}));
  REQUIRE(result[2] == std::set<int>({1}));
  REQUIRE(result[3] == std::set<int>({0, 1}));
  REQUIRE(result[4] == std::set<int>({2}));
  REQUIRE(result[5] == std::set<int>({0, 2}));
  REQUIRE(result[6] == std::set<int>({1, 2}));
  REQUIRE(result[7] == std::set<int>({0, 1, 2}));
}

TEST_CASE("Bit length", "[bit_length]") {
  REQUIRE(bit_length(0) == 0);
  REQUIRE(bit_length(1) == 1);
  REQUIRE(bit_length(2) == 2);
  REQUIRE(bit_length(3) == 2);
  REQUIRE(bit_length(4) == 3);
  REQUIRE(bit_length(8) == 4);
}

TEST_CASE("binary vector/string to int", "[bin2state]") {
  REQUIRE(bin2state(std::vector<int>{0}) == 0);
  REQUIRE(bin2state(std::vector<int>{1}) == 1);
  REQUIRE(bin2state(std::vector<int>{1, 0}) == 2);
  REQUIRE(bin2state(std::vector<int>{1, 1}) == 3);
  REQUIRE(bin2state(std::vector<int>{0, 0, 1}) == 1);
  REQUIRE(bin2state(std::vector<int>{0, 0, 1, 0, 0}) == 4);

  REQUIRE(bin2state("0") == 0);
  REQUIRE(bin2state("1") == 1);
  REQUIRE(bin2state("10") == 2);
  REQUIRE(bin2state("11") == 3);
  REQUIRE(bin2state("001") == 1);
  REQUIRE(bin2state("00100") == 4);
}

TEST_CASE("DFA to Graphviz", "[dfa_transform]") {
  auto mgr = CUDD::Cudd();
  auto my_dfa = dfa(mgr, 1, 1);

  my_dfa.add_state();
  my_dfa.add_transition(0, interpretation_set{}, 1);
  my_dfa.add_transition(1, interpretation_set{}, 1);
  my_dfa.set_final_state(1, true);

  dfa_to_graphviz(my_dfa, "output.svg", "svg");
}

TEST_CASE("DFA to HOA", "[dfa_transform]") {
  Logger log("DFA to HOA");
  auto mgr = CUDD::Cudd();
  auto my_dfa = dfa(mgr, 1, 1);

  my_dfa.add_state();
  my_dfa.add_transition(0, interpretation_set{}, 1);
  my_dfa.add_transition(1, interpretation_set{}, 1);
  my_dfa.set_final_state(1, true);

  std::stringstream o;
  dfa_to_hoa(my_dfa, o);

  auto output = o.str();
  REQUIRE(output.length() > 0);
  log.debug(output);
}

} // namespace whitemech::lydia::Test