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
#include <iostream>
#include <utils/compare.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Boolean atoms", "[logic]") {
  Logger log("test_logic");

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
}

TEST_CASE("LDLfNot", "[logic]") {

  auto ptr_true = std::make_shared<LDLfBooleanAtom>(true);
  auto not_true = LDLfNot(ptr_true);
  auto ptr_false = std::make_shared<LDLfBooleanAtom>(false);
  auto not_false = LDLfNot(ptr_false);

  SECTION("test canonical exception") {
    REQUIRE_THROWS(LDLfNot(std::make_shared<LDLfNot>(ptr_true)));
  }
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

TEST_CASE("And", "[logic]") {
  set_formulas and_1_args = {};
  set_formulas and_2_args = set_formulas();
  set_formulas and_3_args = {boolean(true), boolean(true), boolean(false)};

  SECTION("test exception for number of args") {
    REQUIRE_THROWS(LDLfAnd(and_1_args));
    REQUIRE_THROWS(LDLfAnd(and_2_args));
  }

  auto and_3 = LDLfAnd(and_3_args);
  auto and_3_p = LDLfAnd(and_3_args);

  SECTION("test equality on same object") { REQUIRE(and_3.is_equal(and_3)); }
  SECTION("test equality on different object") {
    REQUIRE(and_3_p.is_equal(and_3_p));
  }
  SECTION("test compare  on same object") {
    REQUIRE(and_3.compare(and_3) == 0);
  }
  SECTION("test compare  on different object") {
    REQUIRE(and_3.compare(and_3_p) == 0);
  }
}

TEST_CASE("LDLfOr", "[logic]") {
  set_formulas or_1_args = {};
  set_formulas or_2_args = set_formulas();
  set_formulas or_3_args = {boolean(true), boolean(true), boolean(false)};

  SECTION("test exception for number of args") {
    REQUIRE_THROWS(LDLfOr(or_1_args));
    REQUIRE_THROWS(LDLfOr(or_2_args));
  }

  auto or_3 = LDLfOr(or_3_args);
  auto or_3_p = LDLfOr(or_3_args);

  SECTION("test equality on same object") { REQUIRE(or_3.is_equal(or_3)); }
  SECTION("test equality on different object") {
    REQUIRE(or_3.is_equal(or_3_p));
  }
  SECTION("test compare  on same object") { REQUIRE(or_3.compare(or_3) == 0); }
}

TEST_CASE("Logical not", "[logic]") {
  auto tt = std::make_shared<LDLfBooleanAtom>(true);
  auto ff = std::make_shared<LDLfBooleanAtom>(false);

  REQUIRE(tt->logical_not()->is_equal(*ff));
  REQUIRE(ff->logical_not()->is_equal(*tt));

  SECTION("De Morgan's Law and-or") {
    set_formulas args_1_and = {tt, ff};
    set_formulas args_2_and = {ff, tt};
    auto and_ = std::make_shared<LDLfAnd>(args_1_and);
    auto actual_or = and_->logical_not();
    auto expected_or = std::make_shared<LDLfOr>(args_2_and);
    REQUIRE(actual_or->is_equal(*expected_or));
  }

  SECTION("De Morgan's Law or-and") {
    set_formulas args_1_and = {tt, ff};
    set_formulas args_2_and = {ff, tt};
    auto or_ = std::make_shared<LDLfOr>(args_1_and);
    auto expected_and = or_->logical_not();
    auto actual_and = std::make_shared<LDLfAnd>(args_2_and);
    REQUIRE(actual_and->is_equal(*expected_and));
  }
}

TEST_CASE("LDLfDiamond", "[logic]") {
  auto true_ = std::make_shared<const PropositionalTrue>();
  auto tt = boolean(true);
  auto false_ = std::make_shared<const PropositionalFalse>();
  auto ff = boolean(false);
  auto a = std::make_shared<const PropositionalAtom>("a");
  auto b = std::make_shared<const PropositionalAtom>("b");
  auto a_and_b =
      std::make_shared<const PropositionalAnd>(set_prop_formulas{a, b});

  auto regex_true = std::make_shared<const PropositionalRegExp>(true_);
  auto diamond_formula_true_tt =
      std::make_shared<LDLfDiamond<PropositionalRegExp>>(regex_true, tt);

  auto regex_false = std::make_shared<const PropositionalRegExp>(false_);
  auto diamond_formula_false_tt =
      std::make_shared<LDLfDiamond<PropositionalRegExp>>(regex_false, tt);

  auto regex_a = std::make_shared<const PropositionalRegExp>(a);
  auto diamond_formula_a_tt =
      std::make_shared<LDLfDiamond<PropositionalRegExp>>(regex_a, tt);
  auto regex_b = std::make_shared<const PropositionalRegExp>(b);
  auto diamond_formula_b_tt =
      std::make_shared<LDLfDiamond<PropositionalRegExp>>(regex_b, tt);
  auto regex_a_and_b = std::make_shared<const PropositionalRegExp>(a_and_b);
  auto diamond_formula_a_and_b_tt =
      std::make_shared<LDLfDiamond<PropositionalRegExp>>(regex_a_and_b, tt);

  REQUIRE(*diamond_formula_true_tt == *diamond_formula_true_tt);
  REQUIRE(*diamond_formula_true_tt != *diamond_formula_false_tt);
  REQUIRE(*diamond_formula_true_tt < *diamond_formula_false_tt);
  REQUIRE(*diamond_formula_false_tt < *diamond_formula_a_tt);
  REQUIRE(*diamond_formula_a_tt < *diamond_formula_b_tt);
  REQUIRE(*diamond_formula_b_tt < *diamond_formula_a_and_b_tt);
  REQUIRE(*diamond_formula_a_and_b_tt == *diamond_formula_a_and_b_tt);
}

TEST_CASE("Set of formulas", "[logic]") {
  auto tt = std::make_shared<const LDLfBooleanAtom>(true);
  auto ff = std::make_shared<const LDLfBooleanAtom>(false);
  set_formulas args = {boolean(true), boolean(false)};
  auto and_ = std::make_shared<const LDLfAnd>(args);
  auto or_ = std::make_shared<const LDLfOr>(args);

  REQUIRE(*ff < *tt);
  REQUIRE(*tt < *and_);
  REQUIRE(*and_ < *or_);
  REQUIRE(!(*or_ < *and_));

  set_formulas formulas = set_formulas({and_, or_, tt, ff});
  CHECK(true);
  // check order. Vectorize first
  vec_formulas result(formulas.begin(), formulas.end());

  REQUIRE(result[0]->is_equal(*ff));
  REQUIRE(result[1]->is_equal(*tt));
  REQUIRE(result[2]->is_equal(*and_));
  REQUIRE(result[3]->is_equal(*or_));
}

} // namespace whitemech::lydia::Test