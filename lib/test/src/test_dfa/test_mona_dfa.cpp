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

#include "../utils/to_dfa.hpp"
#include <catch.hpp>
#include <iostream>
#include <lydia/dfa/mona_dfa.hpp>

namespace whitemech::lydia::Test {

void _print_mona_dfa(DFA *a, const std::string &name, int num = 1) {
  std::vector<unsigned> x(num);
  std::iota(std::begin(x), std::end(x), 0);
  std::ofstream out;
  out.open(name + ".dot", std::ofstream::out);
  dfaPrintGraphvizToFile(a, num, x.data(), out);
  out.close();
  std::system(
      std::string("dot -Tsvg " + name + ".dot > " + name + ".svg").c_str());
}

TEST_CASE("Test MONA dfaLDLfTrue", "[dfa/mona_dfa/true]") {
  bdd_init();
  auto automaton = mona_dfa(dfaLDLfTrue());
  REQUIRE(automaton.get_nb_states() == 1);
  REQUIRE(automaton.get_nb_variables() == 0);
  REQUIRE(verify(automaton, {}, true));
  REQUIRE(verify(automaton, {""}, true));
  REQUIRE(verify(automaton, {"", ""}, true));
}

TEST_CASE("Test MONA dfaLDLfFalse", "[dfa/mona_dfa/true]") {
  bdd_init();
  auto automaton = mona_dfa(dfaLDLfFalse());
  REQUIRE(automaton.get_nb_states() == 1);
  REQUIRE(automaton.get_nb_variables() == 0);
  REQUIRE(verify(automaton, {}, false));
  REQUIRE(verify(automaton, {""}, false));
  REQUIRE(verify(automaton, {"", ""}, false));
}

TEST_CASE("Test MONA dfaNext", "[dfa/mona_dfa/next]") {
  bdd_init();
  auto a = mona_dfa(dfaNext(0));
  _print_mona_dfa(a.get_dfa(), "next", 1);
}

TEST_CASE("Test MONA dfaLDLfProp", "[dfa/mona_dfa/prop]") {
  bdd_init();
  int var = 1;
  auto l = std::vector<int>(var);
  std::iota(l.begin(), l.end(), 0);
  auto ttrue = mona_dfa(dfaLDLfTrue());
  auto result = mona_dfa(dfaLDLfDiamondProp(0, ttrue.get_dfa(), var, l.data()));
  _print_mona_dfa(result.get_dfa(), "dfaLDLfProp", 4);
  REQUIRE(verify(result, {}, false));
  REQUIRE(verify(result, {"0"}, false));
  REQUIRE(verify(result, {"1"}, true));
  REQUIRE(verify(result, {"0", "0"}, false));
  REQUIRE(verify(result, {"0", "1"}, false));
  REQUIRE(verify(result, {"1", "0"}, true));
  REQUIRE(verify(result, {"1", "1"}, true));
}

} // namespace whitemech::lydia::Test