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
#include <lydia/logger.hpp>
#include <lydia/logic/symbol.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Symbol", "[symbol]") {
  Logger log("test_symbol");
  auto context = AstManager();
  auto x = context.makeSymbol("x");
  auto y = context.makeSymbol("y");

  auto newX = context.makeSymbol("x");
  auto newY = context.makeSymbol("y");

  SECTION("x equal to x") { REQUIRE(x->is_equal(*x)); }
  SECTION("x equal to x using ==") { REQUIRE(*x == *x); }
  SECTION("x not equal to y") { REQUIRE(!x->is_equal(*y)); }
  SECTION("x not equal to y using !=") { REQUIRE(*x != *y); }

  SECTION("x equal to newX") { REQUIRE(x->is_equal(*newX)); }
  SECTION("y equal to newY") { REQUIRE(y->is_equal(*newY)); }

  SECTION("x.name") { REQUIRE(x->get_name() == "x"); }
  SECTION("y.name") { REQUIRE(y->get_name() == "y"); }

  SECTION("x.hash() == x.hash()") { REQUIRE(x->hash() == x->hash()); }
  SECTION("y.hash() == y.hash()") { REQUIRE(y->hash() == y->hash()); }
  SECTION("x.hash() != y.hash()") { REQUIRE(x->hash() != y->hash()); }
  SECTION("x.hash() == newX.hash()") { REQUIRE(x->hash() == newX->hash()); }
  SECTION("y.hash() == newY.hash()") { REQUIRE(y->hash() == newY->hash()); }

  SECTION("x < y") { REQUIRE(x->compare_(*y) == -1); }
  SECTION("y > x") { REQUIRE(y->compare_(*x) == 1); }
}

} // namespace whitemech::lydia::Test