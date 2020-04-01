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
#include "symbol.hpp"

namespace whitemech::lydia::Test {

TEST_CASE("Symbol", "[symbol]") {
  Logger log("test_symbol");

  auto x = symbol("x");
  auto y = symbol("y");

  SECTION("x equal to x") { REQUIRE(x.is_equal(x)); }
  SECTION("x equal to x using ==") { REQUIRE(x == x); }
  SECTION("x not equal to y") { REQUIRE(!x.is_equal(y)); }
  SECTION("x not equal to y using !=") { REQUIRE(x != y); }

  SECTION("x.name") { REQUIRE(x.name() == "x"); }
  SECTION("y.name") { REQUIRE(y.name() == "y"); }

  SECTION("x.hash") { REQUIRE(x.hash() != y.hash()); }
}

} // namespace whitemech::lydia::Test