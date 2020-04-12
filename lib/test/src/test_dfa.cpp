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
#include <cuddObj.hh>
#include <dfa.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Test Cudd", "[cudd]") {
  CUDD::Cudd mgr(0, 0);
  CUDD::BDD x = mgr.bddVar();
  CUDD::BDD y = mgr.bddVar();
  CUDD::BDD not_x = !x;
  CUDD::BDD x_and_y = x & y;
  CUDD::BDD x_or_y = x | y;
  CUDD::BDD x_times_y = x * y;
  CUDD::BDD x_plus_y = x + y;
  CUDD::BDD f = x * y;
  CUDD::BDD g = y + !x;

  SECTION("Test comparison") {
    REQUIRE(f <= g);
    REQUIRE(not((bool)f > g));
  }

  int input00[] = {0, 0};
  int input01[] = {0, 1};
  int input10[] = {1, 0};
  int input11[] = {1, 1};

  SECTION("Test evaluation x & y") {
    REQUIRE(x_and_y.Eval(input00).IsZero());
    REQUIRE(x_and_y.Eval(input01).IsZero());
    REQUIRE(x_and_y.Eval(input10).IsZero());
    REQUIRE(x_and_y.Eval(input11).IsOne());
  }

  SECTION("Test evaluation x | y") {
    REQUIRE(x_or_y.Eval(input00).IsZero());
    REQUIRE(x_or_y.Eval(input01).IsOne());
    REQUIRE(x_or_y.Eval(input10).IsOne());
    REQUIRE(x_or_y.Eval(input11).IsOne());
  }

  SECTION("Test evaluation x | y") {
    REQUIRE(x_or_y.Eval(input00).IsZero());
    REQUIRE(x_or_y.Eval(input01).IsOne());
    REQUIRE(x_or_y.Eval(input10).IsOne());
    REQUIRE(x_or_y.Eval(input11).IsOne());
  }

  SECTION("Test evaluation !x") {
    REQUIRE(not_x.Eval(input00).IsOne());
    REQUIRE(not_x.Eval(input01).IsOne());
    REQUIRE(not_x.Eval(input10).IsZero());
    REQUIRE(not_x.Eval(input11).IsZero());
  }

  SECTION("Test evaluation x * y") {
    REQUIRE(x_times_y.Eval(input00).IsZero());
    REQUIRE(x_times_y.Eval(input01).IsZero());
    REQUIRE(x_times_y.Eval(input10).IsZero());
    REQUIRE(x_times_y.Eval(input11).IsOne());
  }

  SECTION("Test evaluation x + y") {
    REQUIRE(x_plus_y.Eval(input00).IsZero());
    REQUIRE(x_plus_y.Eval(input01).IsOne());
    REQUIRE(x_plus_y.Eval(input10).IsOne());
    REQUIRE(x_plus_y.Eval(input11).IsOne());
  }

  CUDD::BDD zero = mgr.bddZero();
  CUDD::BDD one = mgr.bddZero();
}

TEST_CASE("Test DFA initialization", "[dfa]") {
  whitemech::lydia::Logger::level(LogLevel::debug);
  SECTION("Initialize without Cudd manager.") {
    auto my_dfa = // NOLINT
        dfa::read_from_file("../../../lib/test/src/data/mona/eventually_a.dfa");
  }

  SECTION("Initialize with Cudd manager.") {
    auto mgr = new CUDD::Cudd();
    auto my_dfa = // NOLINT
        dfa::read_from_file("../../../lib/test/src/data/mona/eventually_a.dfa",
                            mgr = mgr);
  }
}

TEST_CASE("Test bdd2dot", "[dfa]") {
  whitemech::lydia::Logger::level(LogLevel::debug);
  auto my_dfa =
      dfa::read_from_file("../../../lib/test/src/data/mona/mona_example.dfa");
  my_dfa->bdd2dot();
}

TEST_CASE("Test accepts", "[dfa]") {

  whitemech::lydia::Logger::level(LogLevel::debug);

  interpretation empty = {0};
  interpretation a = {1};
  interpretation na = {0};

  // for MONA DFAs, the first is ignored
  // we will put a dummy symbol at the beginning of every trace.
  auto t_ = trace{empty};
  auto t_a = trace{empty, a};
  auto t_na = trace{empty, na};
  auto t_na_na = trace{empty, na, na};
  auto t_na_a = trace{empty, na, a};
  auto t_a_na = trace{empty, a, na};
  auto t_a_a = trace{empty, a, a};

  SECTION("Test F(a)") {
    auto my_dfa =
        dfa::read_from_file("../../../lib/test/src/data/mona/eventually_a.dfa");

    REQUIRE(!my_dfa->accepts(t_));
    REQUIRE(my_dfa->accepts(t_a));
    REQUIRE(!my_dfa->accepts(t_na_na));
    REQUIRE(my_dfa->accepts(t_na_a));
    REQUIRE(my_dfa->accepts(t_a_na));
    REQUIRE(my_dfa->accepts(t_a_a));
  }

  SECTION("Test G(a)") {
    auto my_dfa =
        dfa::read_from_file("../../../lib/test/src/data/mona/always_a.dfa");

    REQUIRE(!my_dfa->accepts(t_));
    REQUIRE(my_dfa->accepts(t_a));
    REQUIRE(!my_dfa->accepts(t_na_na));
    REQUIRE(!my_dfa->accepts(t_na_a));
    REQUIRE(!my_dfa->accepts(t_a_na));
    REQUIRE(my_dfa->accepts(t_a_a));
  }
}

} // namespace whitemech::lydia::Test
