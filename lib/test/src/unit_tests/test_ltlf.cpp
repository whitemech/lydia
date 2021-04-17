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
#include <iostream>
#include <lydia/logger.hpp>
#include <lydia/logic/ltlf/base.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("LTLf Boolean atoms", "[logic][ltlf]") {
  Logger log("test_logic");

  auto context = AstManager{};
  auto boolTrue = context.makeLtlfTrue();
  auto boolFalse = context.makeLtlfFalse();
  auto newBoolTrue = context.makeLtlfTrue();
  auto newBoolFalse = context.makeLtlfFalse();

  SECTION("tt == tt") { REQUIRE(*boolTrue == *boolTrue); }
  SECTION("ff == ff") { REQUIRE(*boolFalse == *boolFalse); }
  SECTION("tt != ff") { REQUIRE(*boolTrue != *boolFalse); }
  SECTION("ff != tt") { REQUIRE(*boolFalse != *boolTrue); }
  SECTION("tt == new_tt") { REQUIRE(*boolTrue == *newBoolTrue); }
  SECTION("ff == new_ff") { REQUIRE(*boolFalse == *newBoolFalse); }

  SECTION("tt > tt == 0") { REQUIRE(boolTrue->compare(*boolTrue) == 0); }
  SECTION("tt > ff == -1") { REQUIRE(boolTrue->compare(*boolFalse) == -1); }
  SECTION("ff > tt == 1") { REQUIRE(boolFalse->compare(*boolTrue) == 1); }
  SECTION("ff > ff == 0") { REQUIRE(boolFalse->compare(*boolFalse) == 0); }

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
    REQUIRE(boolTrue->hash() == newBoolTrue->hash());
  }
  SECTION("ff->hash() == new_ff->hash()") {
    REQUIRE(boolFalse->hash() == newBoolFalse->hash());
  }
}

TEST_CASE("LTLfNot", "[logic][ltlf]") {
  auto context = AstManager{};
  auto ptr_true = context.makeLtlfTrue();
  auto not_true = context.makeLtlfNot(ptr_true);
  auto ptr_false = context.makeLtlfFalse();
  auto not_false = context.makeLtlfNot(ptr_false);

  SECTION("test equality on same objects") {
    REQUIRE(not_true->is_equal(*not_true));
    REQUIRE(not_false->is_equal(*not_false));
  }
  SECTION("test inequality") {
    REQUIRE(!not_true->is_equal(*not_false));
    REQUIRE(!not_false->is_equal(*not_true));
  }
  SECTION("test compare") {
    REQUIRE(not_true->compare(*not_true) == 0);
    REQUIRE(not_false->compare(*not_false) == 0);
    REQUIRE(not_true->compare(*not_false) == -1);
    REQUIRE(not_false->compare(*not_true) == 1);
  }
}

TEST_CASE("LTLf And", "[logic][ltlf]") {
  auto context = AstManager{};
  auto boolTrue = context.makeLtlfTrue();
  auto boolFalse = context.makeLtlfFalse();
  set_ltlf_formulas and_1_args = {};
  set_ltlf_formulas and_2_args = set_ltlf_formulas();
  set_ltlf_formulas and_3_args = {boolTrue, boolTrue, boolFalse};

  SECTION("test exception for number of args") {
    REQUIRE_THROWS(LTLfAnd(context, and_1_args));
    REQUIRE_THROWS(LTLfAnd(context, and_2_args));
  }

  auto and_3 = context.makeLtlfAnd(and_3_args);
  auto and_3_p = context.makeLtlfAnd(and_3_args);

  SECTION("test equality on same object") { REQUIRE(and_3->is_equal(*and_3)); }
  SECTION("test equality on different object") {
    REQUIRE(and_3_p->is_equal(*and_3_p));
  }
  SECTION("test compare  on same object") {
    REQUIRE(and_3->compare(*and_3) == 0);
  }
  SECTION("test compare  on different object") {
    REQUIRE(and_3->compare(*and_3_p) == 0);
  }
}
//
TEST_CASE("LTLfOr", "[logic][ltlf]") {
  auto context = AstManager{};
  auto boolTrue = context.makeLtlfTrue();
  auto boolFalse = context.makeLtlfFalse();
  set_ltlf_formulas or_1_args = {};
  set_ltlf_formulas or_2_args = set_ltlf_formulas();
  set_ltlf_formulas or_3_args = {boolTrue, boolTrue, boolFalse};

  SECTION("test exception for number of args") {
    REQUIRE_THROWS(LTLfOr(context, or_1_args));
    REQUIRE_THROWS(LTLfOr(context, or_2_args));
  }

  auto or_3 = context.makeLtlfOr(or_3_args);
  auto or_3_p = context.makeLtlfOr(or_3_args);

  SECTION("test equality on same object") { REQUIRE(or_3->is_equal(*or_3)); }
  SECTION("test equality on different object") {
    REQUIRE(or_3->is_equal(*or_3_p));
  }
  SECTION("test compare  on same object") {
    REQUIRE(or_3->compare(*or_3) == 0);
  }
}

TEST_CASE("LTLf Logical not", "[logic][ltlf]") {
  auto context = AstManager{};
  auto tt = context.makeLtlfTrue();
  auto ff = context.makeLtlfFalse();

  REQUIRE(tt->logical_not()->is_equal(*ff));
  REQUIRE(ff->logical_not()->is_equal(*tt));

  SECTION("De Morgan's Law and-or") {
    set_ltlf_formulas args_1_and = {tt, ff};
    set_ltlf_formulas args_2_and = {ff, tt};
    auto and_ = context.makeLtlfAnd(args_1_and);
    auto actual_or = and_->logical_not();
    auto expected_or = context.makeLtlfOr(args_2_and);
    REQUIRE(actual_or->is_equal(*expected_or));
  }

  SECTION("De Morgan's Law or-and") {
    set_ltlf_formulas args_1_and = {tt, ff};
    set_ltlf_formulas args_2_and = {ff, tt};
    auto or_ = context.makeLtlfOr(args_1_and);
    auto expected_and = or_->logical_not();
    auto actual_and = context.makeLtlfAnd(args_2_and);
    REQUIRE(actual_and->is_equal(*expected_and));
  }
}

TEST_CASE("LTLf Set of formulas", "[logic][ltlf]") {
  auto context = AstManager{};
  auto tt = context.makeLtlfTrue();
  auto ff = context.makeLtlfFalse();
  auto a = context.makeLtlfAtom("a");
  auto b = context.makeLtlfAtom("b");
  set_ltlf_formulas args = {a, b};
  auto and_ = context.makeLtlfAnd(args);
  auto or_ = context.makeLtlfOr(args);

  REQUIRE(*tt < *ff);
  REQUIRE(*ff < *and_);
  REQUIRE(*and_ < *or_);
  REQUIRE(!(*or_ < *and_));

  set_ltlf_formulas formulas = set_ltlf_formulas({and_, or_, tt, ff});

  // check order. Vectorize first
  vec_ltlf_formulas result(formulas.begin(), formulas.end());

  REQUIRE(result[0]->is_equal(*tt));
  REQUIRE(result[1]->is_equal(*ff));
  REQUIRE(result[2]->is_equal(*and_));
  REQUIRE(result[3]->is_equal(*or_));
}

} // namespace whitemech::lydia::Test