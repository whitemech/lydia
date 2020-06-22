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
#include <lydia/to_dfa/strategies/sat.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("prime implicants", "[prime_implicants]") {

  SECTION("true") {
    auto true_ = boolean_prop(true);
    auto prime_implicants = all_prime_implicants(*true_);
    REQUIRE(prime_implicants.size() == 1);
    REQUIRE(prime_implicants[0].size() == 0);
  }
  SECTION("false") {
    auto true_ = boolean_prop(false);
    auto prime_implicants = all_prime_implicants(*true_);
    REQUIRE(prime_implicants.size() == 0);
  }
  SECTION("a") {
    auto a = prop_atom("a");
    auto prime_implicants = all_prime_implicants(*a);
    REQUIRE(prime_implicants.size() == 1);
    REQUIRE(prime_implicants[0].size() == 1);
    auto a_ = *prime_implicants[0].begin();
    REQUIRE(*quote(a) == *a_->symbol);
  }
  SECTION("not a") {
    auto f = logical_not(prop_atom("not a"));
    auto prime_implicants = all_prime_implicants(*f);
    REQUIRE(prime_implicants.size() == 1);
    REQUIRE(prime_implicants[0].size() == 1);
  }
  SECTION("a and b") {
    auto a = prop_atom("a");
    auto b = prop_atom("b");
    auto f = logical_and({a, b});
    auto prime_implicants = all_prime_implicants(*f);
    REQUIRE(prime_implicants.size() == 1);
    REQUIRE(prime_implicants[0].size() == 2);
  }
  SECTION("a or b") {
    auto a = prop_atom("a");
    auto b = prop_atom("b");
    auto f = logical_or({a, b});
    auto prime_implicants = all_prime_implicants(*f);
    REQUIRE(prime_implicants.size() == 2);
    REQUIRE(prime_implicants[0].size() == 1);
    REQUIRE(prime_implicants[1].size() == 1);
  }
  SECTION("c or (a and b)") {
    auto a = prop_atom("a");
    auto b = prop_atom("b");
    auto c = prop_atom("c");
    auto f = logical_or({c, logical_and({logical_not(a), logical_not(b)})});
    auto prime_implicants = all_prime_implicants(*f);
    REQUIRE(prime_implicants.size() == 2);
    REQUIRE(prime_implicants[0].size() == 2); // {a, b}
    REQUIRE(prime_implicants[1].size() == 1); // {c}
  }
}

} // namespace whitemech::lydia::Test