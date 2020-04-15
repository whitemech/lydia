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
#include "utils/print.hpp"

namespace whitemech::lydia::Test {

TEST_CASE("string printer", "[string_printer]") {

  SECTION("test tt.str()") {
    StrPrinter strPrinter;
    auto f = LDLfBooleanAtom(true);
    auto expected = "tt";
    auto actual = strPrinter.apply(f);
    REQUIRE(actual == expected);
  }

  SECTION("test ff.str()") {
    auto f = LDLfBooleanAtom(false);
    auto expected = "ff";
    auto actual = to_string(f);
    REQUIRE(actual == expected);
  }

  SECTION("to string tt & ff") {
    auto f = LDLfAnd({boolean(true), boolean(false)});
    auto expected = "And(ff, tt)";
    auto actual = to_string(f);
    REQUIRE(actual == expected);
  }

  SECTION("to string tt | ff") {
    StrPrinter strPrinter;
    auto f = LDLfOr({boolean(true), boolean(false)});
    auto expected = "Or(ff, tt)";
    auto actual = to_string(f);
    REQUIRE(actual == expected);
  }

  SECTION("to string !tt") {
    StrPrinter strPrinter;
    auto f = LDLfNot(boolTrue);
    auto expected = "Not(tt)";
    auto actual = to_string(f);
    REQUIRE(actual == expected);
  }
}

} // namespace whitemech::lydia::Test