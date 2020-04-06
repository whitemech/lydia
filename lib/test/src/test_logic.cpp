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
#include "logic.hpp"

namespace whitemech::lydia::Test {

TEST_CASE("Logic", "[LDLfBooleanAtom]") {
  Logger log("test_logic");

  auto newBoolTrue = LDLfBooleanAtom(true);
  auto newBoolFalse = LDLfBooleanAtom(false);

  SECTION("tt == tt") { REQUIRE(boolTrue == boolTrue); }
  SECTION("ff == ff") { REQUIRE(boolFalse == boolFalse); }
  SECTION("tt != ff") { REQUIRE(boolTrue != boolFalse); }
  SECTION("ff != tt") { REQUIRE(boolFalse != boolTrue); }
  SECTION("tt == new_tt") { REQUIRE(boolTrue == newBoolTrue); }
  SECTION("ff == new_ff") { REQUIRE(boolFalse == newBoolFalse); }

  SECTION("tt > tt == 0") { REQUIRE(boolTrue.compare(boolTrue) == 0); }
  SECTION("tt > ff == 1") { REQUIRE(boolTrue.compare(boolFalse) == 1); }
  SECTION("ff < tt == -1") { REQUIRE(boolFalse.compare(boolTrue) == -1); }
  SECTION("ff < ff == 0") { REQUIRE(boolFalse.compare(boolFalse) == 0); }

  SECTION("tt.hash() == tt.hash()") {
    REQUIRE(boolTrue.hash() == boolTrue.hash());
  }
  SECTION("tt.hash() != tt.hash()") {
    REQUIRE(boolTrue.hash() != boolFalse.hash());
  }
  SECTION("ff.hash() == ff.hash()") {
    REQUIRE(boolFalse.hash() == boolFalse.hash());
  }
  SECTION("tt.hash() == new_tt.hash()") {
    REQUIRE(boolTrue.hash() == newBoolTrue.hash());
  }
  SECTION("ff.hash() == new_ff.hash()") {
    REQUIRE(boolFalse.hash() == newBoolFalse.hash());
  }

  // TODO test to string
}
} // namespace whitemech::lydia::Test