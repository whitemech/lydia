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

TEST_CASE("Boolean atoms", "[LDLfBooleanAtom]") {
  Logger log("test_logic");

  auto boolTrue = boolean(true);
  auto boolFalse = boolean(false);
  auto newBoolTrue = LDLfBooleanAtom(true);
  auto newBoolFalse = LDLfBooleanAtom(false);

  SECTION("tt == tt") { REQUIRE(*boolTrue == *boolTrue); }
  SECTION("ff == ff") { REQUIRE(*boolFalse == *boolFalse); }
  SECTION("tt != ff") { REQUIRE(*boolTrue != *boolFalse); }
  SECTION("ff != tt") { REQUIRE(*boolFalse != *boolTrue); }
  SECTION("tt == new_tt") { REQUIRE(*boolTrue == newBoolTrue); }
  SECTION("ff == new_ff") { REQUIRE(*boolFalse == newBoolFalse); }

  SECTION("tt > tt == 0") { REQUIRE(boolTrue->compare(*boolTrue) == 0); }
  SECTION("tt > ff == 1") { REQUIRE(boolTrue->compare(*boolFalse) == 1); }
  SECTION("ff < tt == -1") { REQUIRE(boolFalse->compare(*boolTrue) == -1); }
  SECTION("ff < ff == 0") { REQUIRE(boolFalse->compare(*boolFalse) == 0); }

  SECTION("tt.hash() == tt.hash()") {
    REQUIRE(boolTrue->hash() == boolTrue->hash());
  }
  SECTION("tt->hash() != tt->hash()") {
    REQUIRE(boolTrue->hash() != boolFalse->hash());
  }
  SECTION("ff->hash() == ff->hash()") {
    REQUIRE(boolFalse->hash() == boolFalse->hash());
  }
  SECTION("tt->hash() == new_tt->hash()") {
    REQUIRE(boolTrue->hash() == newBoolTrue.hash());
  }
  SECTION("ff->hash() == new_ff->hash()") {
    REQUIRE(boolFalse->hash() == newBoolFalse.hash());
  }

  // TODO test to string
}

TEST_CASE("Not", "[Not]") {

  auto ptr_true = std::make_shared<LDLfBooleanAtom>(true);
  auto not_true = Not(ptr_true);
  auto ptr_false = std::make_shared<LDLfBooleanAtom>(false);
  auto not_false = Not(ptr_false);

  SECTION("test equality on same objects") {
    REQUIRE(not_true.is_equal(not_true));
    REQUIRE(not_false.is_equal(not_false));
  }

  SECTION("test inequality") {
    REQUIRE(!not_true.is_equal(not_false));
    REQUIRE(!not_false.is_equal(not_true));
  }

  SECTION("test compare") {
    REQUIRE(not_true.compare(not_true) == 0);
    REQUIRE(not_false.compare(not_false) == 0);
    REQUIRE(not_true.compare(not_false) == 1);
    REQUIRE(not_false.compare(not_true) == -1);
  }
}

TEST_CASE("And", "[And]") {
  set_formulas and_1_args = {};
  set_formulas and_2_args = set_formulas();
  set_formulas and_3_args = {boolean(true), boolean(true), boolean(false)};

  auto and_1 =
      And(and_1_args); // TODO: this should raise exception: less than two
  auto and_2 = And(and_2_args); // TODO see above
  auto and_3 = And(and_3_args);

  SECTION("test equality on same object") { REQUIRE(and_3.is_equal(and_3)); }
  SECTION("test compare  on same object") {
    REQUIRE(and_3.compare(and_3) == 0);
  }
}

TEST_CASE("Or", "[Or]") {
  set_formulas or_1_args = {};
  set_formulas or_2_args = set_formulas();
  set_formulas or_3_args = {boolean(true), boolean(true), boolean(false)};

  auto or_1 = Or(or_1_args); // TODO: this should raise exception: less than two
  auto or_2 = Or(or_2_args); // TODO see above
  auto or_3 = Or(or_3_args);

  SECTION("test equality on same object") { REQUIRE(or_3.is_equal(or_3)); }
  SECTION("test compare  on same object") { REQUIRE(or_3.compare(or_3) == 0); }
}

} // namespace whitemech::lydia::Test