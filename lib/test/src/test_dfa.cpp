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
#include "logger.hpp"
#include <bddx.h>
#include <cuddObj.hh>
#include <dfa.hpp>
#include <iostream>

namespace whitemech::lydia::Test {

TEST_CASE("Test Cudd", "[cudd]") {
  CUDD::Cudd mgr(0, 0);
  CUDD::BDD x = mgr.bddVar();
  CUDD::BDD y = mgr.bddVar();
  CUDD::BDD f = x * y;
  CUDD::BDD g = y + !x;

  SECTION("Test comparison") {
    REQUIRE(f <= g);
    REQUIRE(not((bool)f > g));
  }
}

TEST_CASE("Test DFA initialization", "[dfa]") {
  auto my_dfa = // NOLINT
      dfa::read_from_file("lib/test/src/data/mona/eventually_a.dfa"); // NOLINT
  //  my_dfa->bdd2dot();
}

} // namespace whitemech::lydia::Test
