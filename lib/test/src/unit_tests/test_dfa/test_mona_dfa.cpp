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

#include "test/src/utils/to_dfa.hpp"
#include <catch.hpp>
#include <iostream>
#include <lydia/dfa/mona_dfa.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Test is sink", "[dfa][is_sink]") {
  REQUIRE(is_sink(dfaLDLfTrue(), true));
  REQUIRE(!is_sink(dfaLDLfTrue(), false));
  REQUIRE(is_sink(dfaLDLfFalse(), false));
  REQUIRE(!is_sink(dfaLDLfFalse(), true));
}

TEST_CASE("Test MONA dfa_concatenate", "[dfa][mona_dfa][concatenation]") {
  bdd_init();
  int var = 2;
  auto indices = std::vector<int>(var);
  std::iota(indices.begin(), indices.end(), 0);
  auto a = dfaNext(0);
  auto b = dfaNext(1);
  auto automaton = mona_dfa(dfa_concatenate(a, b, var, indices.data()), var);
  dfaFree(a);
  dfaFree(b);
  print_mona_dfa(automaton.get_dfa(), "concatenate_a_b", 4);
  REQUIRE(automaton.get_nb_states() == 4);
  REQUIRE(automaton.get_nb_variables() == 2);
  REQUIRE(verify(automaton, {}, false));
  REQUIRE(verify(automaton, {"00"}, false));
  REQUIRE(verify(automaton, {"10"}, false));
  REQUIRE(verify(automaton, {"01"}, false));
  REQUIRE(verify(automaton, {"11"}, false));
  REQUIRE(verify(automaton, {"00", "00"}, false));
  REQUIRE(verify(automaton, {"00", "01"}, false));
  REQUIRE(verify(automaton, {"00", "10"}, false));
  REQUIRE(verify(automaton, {"00", "11"}, false));
  REQUIRE(verify(automaton, {"01", "00"}, false));
  REQUIRE(verify(automaton, {"01", "01"}, false));
  REQUIRE(verify(automaton, {"01", "10"}, true));
  REQUIRE(verify(automaton, {"01", "11"}, true));
  REQUIRE(verify(automaton, {"10", "00"}, false));
  REQUIRE(verify(automaton, {"10", "01"}, false));
  REQUIRE(verify(automaton, {"10", "10"}, false));
  REQUIRE(verify(automaton, {"10", "11"}, false));
  REQUIRE(verify(automaton, {"11", "00"}, false));
  REQUIRE(verify(automaton, {"11", "01"}, false));
  REQUIRE(verify(automaton, {"11", "10"}, true));
  REQUIRE(verify(automaton, {"11", "11"}, true));
  REQUIRE(verify(automaton, {"11", "11", "11"}, false));
}

TEST_CASE("Test MONA dfa_closure a", "[dfa][mona_dfa][closure]") {
  bdd_init();
  int var = 1;
  auto indices = std::vector<int>(var);
  std::iota(indices.begin(), indices.end(), 0);
  auto a = dfaNext(0);
  auto automaton = mona_dfa(dfa_closure(a, var, indices.data()), var);
  dfaFree(a);
  print_mona_dfa(automaton.get_dfa(), "closure_a", 4);
  REQUIRE(automaton.get_nb_states() == 3);
  REQUIRE(automaton.get_nb_variables() == 1);
}

TEST_CASE("Test MONA dfa_closure a,b and allow empty",
          "[dfa][mona_dfa][closure]") {
  bdd_init();
  int var = 2;
  auto indices = std::vector<int>(var);
  std::iota(indices.begin(), indices.end(), 0);
  auto a = dfaNext(0);
  auto b = dfaNext(1);
  auto ab = dfa_concatenate(a, b, var, indices.data());
  auto ab_or_empty = dfa_accept_empty(ab);
  auto tmp = dfa_closure(ab_or_empty, var, indices.data());
  auto automaton = mona_dfa(tmp, var);
  dfaFree(a);
  dfaFree(b);
  dfaFree(ab);
  dfaFree(ab_or_empty);
  print_mona_dfa(automaton.get_dfa(), "closure_ab_accept_empty", 4);
  REQUIRE(automaton.get_nb_states() == 3);
  REQUIRE(automaton.get_nb_variables() == 2);
  REQUIRE(verify(automaton, {}, true));
  REQUIRE(verify(automaton, {"00"}, false));
  REQUIRE(verify(automaton, {"01", "01"}, false));
  REQUIRE(verify(automaton, {"01", "10"}, true));
  REQUIRE(verify(automaton, {"01", "10", "01"}, false));
  REQUIRE(verify(automaton, {"01", "10", "01", "10"}, true));
}

TEST_CASE("Test MONA dfaLDLfTrue", "[dfa][mona_dfa][true]") {
  bdd_init();
  auto automaton = mona_dfa(dfaLDLfTrue(), 0);
  REQUIRE(automaton.get_nb_states() == 1);
  REQUIRE(automaton.get_nb_variables() == 0);
  REQUIRE(verify(automaton, {}, true));
  REQUIRE(verify(automaton, {""}, true));
  REQUIRE(verify(automaton, {"", ""}, true));
}

TEST_CASE("Test MONA dfaLDLfFalse", "[dfa][mona_dfa][true]") {
  bdd_init();
  auto automaton = mona_dfa(dfaLDLfFalse(), 0);
  REQUIRE(automaton.get_nb_states() == 1);
  REQUIRE(automaton.get_nb_variables() == 0);
  REQUIRE(verify(automaton, {}, false));
  REQUIRE(verify(automaton, {""}, false));
  REQUIRE(verify(automaton, {"", ""}, false));
}

TEST_CASE("Test MONA dfaNext", "[dfa][mona_dfa][next]") {
  bdd_init();
  auto a = mona_dfa(dfaNext(0), 1);
  print_mona_dfa(a.get_dfa(), "next", 1);
}

TEST_CASE("Test MONA dfaLDLfProp", "[dfa][mona_dfa][prop]") {
  bdd_init();
  int var = 1;
  auto l = std::vector<int>(var);
  std::iota(l.begin(), l.end(), 0);
  auto ttrue = dfaLDLfTrue();
  auto a = dfaNext(0);
  auto result = mona_dfa(dfaLDLfDiamondProp(a, ttrue, var, l.data()), 1);
  dfaFree(a);
  dfaFree(ttrue);
  print_mona_dfa(result.get_dfa(), "dfaLDLfProp", 4);
  REQUIRE(verify(result, {}, false));
  REQUIRE(verify(result, {"0"}, false));
  REQUIRE(verify(result, {"1"}, true));
  REQUIRE(verify(result, {"0", "0"}, false));
  REQUIRE(verify(result, {"0", "1"}, false));
  REQUIRE(verify(result, {"1", "0"}, true));
  REQUIRE(verify(result, {"1", "1"}, true));
}

} // namespace whitemech::lydia::Test