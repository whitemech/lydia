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
#include <cuddObj.hh>
#include <lydia/dfa/dfa.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Test Cudd", "[cudd]") {
  CUDD::Cudd mgr(0, 0);
  CUDD::BDD x = mgr.bddVar();
  CUDD::BDD y = mgr.bddVar();
  CUDD::BDD z = mgr.bddVar();
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

  int input0[] = {0};
  int input1[] = {1};
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

  SECTION("Test evaluation x + y with only one truth") {
    //    "Don't care" does not work...
    // REQUIRE(x_plus_y.Eval(input0).IsOne()); //this is non-deterministic
    REQUIRE(x_plus_y.Eval(input1).IsOne());
    REQUIRE(!x_and_y.Eval(input0).IsOne());
    REQUIRE(!x_and_y.Eval(input1).IsOne());
  }

  SECTION("Test when combination is true") {
    REQUIRE((x & !x).IsZero());
    REQUIRE(!(x & !x).IsOne());
    REQUIRE((x | !x).IsOne());
    REQUIRE(!(x | !x).IsZero());
    REQUIRE(!(x.IsZero()));
    REQUIRE(!(x.IsOne()));
    REQUIRE(!((!x).IsZero()));
    REQUIRE(!((!x).IsOne()));
  }

  SECTION("Test Print Minterms") {
    auto tmp = ((x | y) & !z);
    std::vector<std::vector<int>> primes;
    int* cube = nullptr;
    DdGen* g =
        Cudd_FirstPrime(mgr.getManager(), tmp.getNode(), tmp.getNode(), &cube);
    if (g != nullptr) {
      do {
        primes.push_back(std::vector<int>(cube, cube + 3));

      } while (Cudd_NextPrime(g, &cube));
    }

    REQUIRE(primes.size() == 2);
    REQUIRE(primes[0] == std::vector<int>{1, 2, 0});
    REQUIRE(primes[1] == std::vector<int>{2, 1, 0});
  }
}

TEST_CASE("Test DFA initialization", "[dfa]") {
  whitemech::lydia::Logger::level(LogLevel::debug);

  SECTION("Initialize with Cudd manager.") {
    auto mgr = CUDD::Cudd();
    auto my_dfa = dfa::read_from_file( // NOLINT
        "../../../lib/test/src/assets/mona/eventually_a.dfa", mgr);
  }
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
    auto mgr = CUDD::Cudd();
    auto my_dfa = dfa::read_from_file(
        "../../../lib/test/src/assets/mona/eventually_a.dfa", mgr);

    REQUIRE(!my_dfa.accepts(t_));
    REQUIRE(!my_dfa.accepts(t_na));
    REQUIRE(my_dfa.accepts(t_a));
    REQUIRE(!my_dfa.accepts(t_na_na));
    REQUIRE(my_dfa.accepts(t_na_a));
    REQUIRE(my_dfa.accepts(t_a_na));
    REQUIRE(my_dfa.accepts(t_a_a));
  }

  SECTION("Test G(a)") {
    auto mgr = CUDD::Cudd();
    auto my_dfa = dfa::read_from_file(
        "../../../lib/test/src/assets/mona/always_a.dfa", mgr);

    REQUIRE(!my_dfa.accepts(t_));
    REQUIRE(!my_dfa.accepts(t_na));
    REQUIRE(my_dfa.accepts(t_a));
    REQUIRE(!my_dfa.accepts(t_na_na));
    REQUIRE(!my_dfa.accepts(t_na_a));
    REQUIRE(!my_dfa.accepts(t_a_na));
    REQUIRE(my_dfa.accepts(t_a_a));
  }
}

TEST_CASE("Incremental construction", "[dfa]") {
  interpretation a = {1};
  interpretation na = {0};
  auto t_ = trace{};
  auto t_a = trace{a};
  auto t_na = trace{na};
  auto t_na_na = trace{na, na};
  auto t_na_a = trace{na, a};
  auto t_a_na = trace{a, na};
  auto t_a_a = trace{a, a};

  SECTION("The starting DFA does not accept anything") {
    auto mgr = CUDD::Cudd();
    auto my_dfa = dfa(mgr, 10, 1);

    REQUIRE(my_dfa.get_successor(0, t_na[0]) == 0);
    REQUIRE(my_dfa.get_successor(0, t_a[0]) == 0);

    REQUIRE(!my_dfa.accepts(t_));
    REQUIRE(!my_dfa.accepts(t_na));
    REQUIRE(!my_dfa.accepts(t_a));
    REQUIRE(!my_dfa.accepts(t_na_na));
    REQUIRE(!my_dfa.accepts(t_na_a));
    REQUIRE(!my_dfa.accepts(t_a_na));
    REQUIRE(!my_dfa.accepts(t_a_a));
  }

  SECTION("With the state 0 as final will make the DFA accept everything.") {
    auto mgr = CUDD::Cudd();
    auto my_dfa = dfa(mgr, 10, 1);
    my_dfa.set_final_state(0, true);
    REQUIRE(my_dfa.accepts(t_));
    REQUIRE(my_dfa.accepts(t_na));
    REQUIRE(my_dfa.accepts(t_a));
    REQUIRE(my_dfa.accepts(t_na_na));
    REQUIRE(my_dfa.accepts(t_na_a));
    REQUIRE(my_dfa.accepts(t_a_na));
    REQUIRE(my_dfa.accepts(t_a_a));
  }

  SECTION("Create a simple DFA: add new state, make it final, and add a "
          "transition to it.") {
    auto mgr = CUDD::Cudd();
    auto my_dfa = dfa(mgr, 10, 1);

    int new_state = my_dfa.add_state();
    REQUIRE(new_state == 1);
    my_dfa.set_initial_state(new_state);

    REQUIRE(!my_dfa.accepts(t_));
    REQUIRE(!my_dfa.accepts(t_na));
    REQUIRE(!my_dfa.accepts(t_a));
    REQUIRE(!my_dfa.accepts(t_na_na));
    REQUIRE(!my_dfa.accepts(t_na_a));
    REQUIRE(!my_dfa.accepts(t_a_na));
    REQUIRE(!my_dfa.accepts(t_a_a));

    SECTION("... and add a self-loop with dont_care=true") {
      /*
       * In this case, the self-loop has TRUE as guard.
       * That is, with dont_care=true and an empty
       * set of constraints.
       */
      my_dfa.set_final_state(1, true);
      my_dfa.add_transition(1, interpretation_set{}, 1, true);

      REQUIRE(my_dfa.get_successor(0, t_na[0]) == 0);
      REQUIRE(my_dfa.get_successor(0, t_a[0]) == 0);
      REQUIRE(my_dfa.get_successor(1, t_na[0]) == 1);
      REQUIRE(my_dfa.get_successor(1, t_a[0]) == 1);

      REQUIRE(my_dfa.accepts(t_));
      REQUIRE(my_dfa.accepts(t_na));
      REQUIRE(my_dfa.accepts(t_a));
      REQUIRE(my_dfa.accepts(t_na_na));
      REQUIRE(my_dfa.accepts(t_na_a));
      REQUIRE(my_dfa.accepts(t_a_na));
      REQUIRE(my_dfa.accepts(t_a_a));
    }

    SECTION("... and add a self-loop with dont_care=false") {
      /*
       * In this case, the self-loop has OR of NOTs for all the variables
       * This is because the interpretation set is empty and dont_care=false.
       * That means all the propositions not present in the interpretation set
       * have to be necessarily false in order to satisfy the guard.
       */
      my_dfa.set_final_state(1, true);
      my_dfa.add_transition(1, interpretation_set{}, 1, false);

      REQUIRE(my_dfa.get_successor(0, t_na[0]) == 0);
      REQUIRE(my_dfa.get_successor(0, t_a[0]) == 0);
      REQUIRE(my_dfa.get_successor(1, t_na[0]) == 1);
      REQUIRE(my_dfa.get_successor(1, t_a[0]) == 0);

      REQUIRE(my_dfa.accepts(t_));
      REQUIRE(my_dfa.accepts(t_na));
      REQUIRE(!my_dfa.accepts(t_a));
      REQUIRE(my_dfa.accepts(t_na_na));
      REQUIRE(!my_dfa.accepts(t_na_a));
      REQUIRE(!my_dfa.accepts(t_a_na));
      REQUIRE(!my_dfa.accepts(t_a_a));
    }
  }
}

} // namespace whitemech::lydia::Test
