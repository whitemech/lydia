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
#include <lydia/logic/ldlf/base.hpp>
#include <lydia/logic/ldlf/only_test.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("LDLfBoolean atoms", "[logic][ldlf]") {
  Logger log("test_logic");

  auto context = AstManager{};
  auto boolTrue = context.makeLdlfTrue();
  auto boolFalse = context.makeLdlfFalse();
  auto newBoolTrue = context.makeLdlfTrue();
  auto newBoolFalse = context.makeLdlfFalse();

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

TEST_CASE("LDLfNot", "[logic][ldlf]") {
  auto context = AstManager{};
  auto ptr_true = context.makeLdlfTrue();
  auto not_true = context.makeLdlfNot(ptr_true);
  auto ptr_false = context.makeLdlfFalse();
  auto not_false = context.makeLdlfNot(ptr_false);

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

TEST_CASE("LDLfAnd", "[logic][ldlf]") {
  auto context = AstManager{};
  auto boolTrue = context.makeLdlfTrue();
  auto boolFalse = context.makeLdlfFalse();
  set_formulas and_1_args = {};
  set_formulas and_2_args = set_formulas();
  set_formulas and_3_args = {boolTrue, boolTrue, boolFalse};

  SECTION("test exception for number of args") {
    REQUIRE_THROWS(LDLfAnd(context, and_1_args));
    REQUIRE_THROWS(LDLfAnd(context, and_2_args));
  }

  auto and_3 = context.makeLdlfAnd(and_3_args);
  auto and_3_p = context.makeLdlfAnd(and_3_args);

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

TEST_CASE("LDLfOr", "[logic][ldlf]") {
  auto context = AstManager{};
  auto boolTrue = context.makeLdlfTrue();
  auto boolFalse = context.makeLdlfFalse();
  set_formulas or_1_args = {};
  set_formulas or_2_args = set_formulas();
  set_formulas or_3_args = {boolTrue, boolTrue, boolFalse};

  SECTION("test exception for number of args") {
    REQUIRE_THROWS(LDLfOr(context, or_1_args));
    REQUIRE_THROWS(LDLfOr(context, or_2_args));
  }

  auto or_3 = context.makeLdlfOr(or_3_args);
  auto or_3_p = context.makeLdlfOr(or_3_args);

  SECTION("test equality on same object") { REQUIRE(or_3->is_equal(*or_3)); }
  SECTION("test equality on different object") {
    REQUIRE(or_3->is_equal(*or_3_p));
  }
  SECTION("test compare  on same object") {
    REQUIRE(or_3->compare(*or_3) == 0);
  }
}

TEST_CASE("LDLfLogical not", "[logic][ldlf]") {
  auto context = AstManager{};
  auto tt = context.makeLdlfTrue();
  auto ff = context.makeLdlfFalse();

  REQUIRE(tt->logical_not()->is_equal(*ff));
  REQUIRE(ff->logical_not()->is_equal(*tt));

  SECTION("De Morgan's Law and-or") {
    set_formulas args_1_and = {tt, ff};
    set_formulas args_2_and = {ff, tt};
    auto and_ = context.makeLdlfAnd(args_1_and);
    auto actual_or = and_->logical_not();
    auto expected_or = context.makeLdlfOr(args_2_and);
    REQUIRE(actual_or->is_equal(*expected_or));
  }

  SECTION("De Morgan's Law or-and") {
    set_formulas args_1_and = {tt, ff};
    set_formulas args_2_and = {ff, tt};
    auto or_ = context.makeLdlfOr(args_1_and);
    auto expected_and = or_->logical_not();
    auto actual_and = context.makeLdlfAnd(args_2_and);
    REQUIRE(actual_and->is_equal(*expected_and));
  }
}

TEST_CASE("LDLfDiamond", "[logic][ldlf]") {
  auto context = AstManager{};
  auto tt = context.makeLdlfTrue();
  auto ff = context.makeLdlfFalse();
  auto true_ = context.makeTrue();
  auto false_ = context.makeFalse();
  auto a = context.makePropAtom("a");
  auto b = context.makePropAtom("b");
  auto a_and_b = context.makePropAnd(set_prop_formulas{a, b});

  auto regex_true = context.makePropRegex(true_);
  auto diamond_formula_true_tt = context.makeLdlfDiamond(regex_true, tt);

  auto regex_false = context.makePropRegex(false_);
  auto diamond_formula_false_tt = context.makeLdlfDiamond(regex_false, tt);

  auto regex_a = context.makePropRegex(a);
  auto diamond_formula_a_tt = context.makeLdlfDiamond(regex_a, tt);
  auto regex_b = context.makePropRegex(b);
  auto diamond_formula_b_tt = context.makeLdlfDiamond(regex_b, tt);
  auto regex_a_and_b = context.makePropRegex(a_and_b);
  auto diamond_formula_a_and_b_tt = context.makeLdlfDiamond(regex_a_and_b, tt);

  REQUIRE(*diamond_formula_true_tt == *diamond_formula_true_tt);
  REQUIRE(*diamond_formula_true_tt != *diamond_formula_false_tt);
  REQUIRE(*diamond_formula_true_tt < *diamond_formula_false_tt);
  REQUIRE(*diamond_formula_false_tt < *diamond_formula_a_tt);
  REQUIRE(*diamond_formula_a_tt < *diamond_formula_b_tt);
  REQUIRE(*diamond_formula_b_tt < *diamond_formula_a_and_b_tt);
  REQUIRE(*diamond_formula_a_and_b_tt == *diamond_formula_a_and_b_tt);
}

TEST_CASE("LDLfSet of formulas", "[logic][ldlf]") {
  auto context = AstManager{};
  auto tt = context.makeLdlfTrue();
  auto ff = context.makeLdlfFalse();
  auto a = context.makeLdlfDiamond(
      context.makePropRegex(context.makePropAtom("a")), tt);
  auto b = context.makeLdlfDiamond(
      context.makePropRegex(context.makePropAtom("b")), tt);
  set_formulas args = {a, b};
  auto and_ = context.makeLdlfAnd(args);
  auto or_ = context.makeLdlfOr(args);

  REQUIRE(*tt < *ff);
  REQUIRE(*ff < *and_);
  REQUIRE(*and_ < *or_);
  REQUIRE(!(*or_ < *and_));

  set_formulas formulas = set_formulas({and_, or_, tt, ff});
  CHECK(true);
  // check order. Vectorize first
  vec_formulas result(formulas.begin(), formulas.end());

  REQUIRE(result[0]->is_equal(*tt));
  REQUIRE(result[1]->is_equal(*ff));
  REQUIRE(result[2]->is_equal(*and_));
  REQUIRE(result[3]->is_equal(*or_));
}

TEST_CASE("LDLf Test 'only test'", "[ldlf/only_test]") {
  auto context = AstManager{};
  auto boolTrue = context.makeLdlfTrue();
  auto boolFalse = context.makeLdlfFalse();
  auto a = context.makePropAtom("a");
  SECTION("Test propositional regex") {
    auto r = context.makePropRegex(a);
    REQUIRE(is_test_only(*r) == false);
  }
  SECTION("Test test regex") {
    auto r = context.makeTestRegex(boolTrue);
    REQUIRE(is_test_only(*r) == true);
  }
  SECTION("Test seq regex positive") {
    auto r1 = context.makeTestRegex(boolTrue);
    auto r2 = context.makeTestRegex(boolFalse);
    auto seq = context.makeSeqRegex({r1, r2});
    REQUIRE(is_test_only(*seq) == true);
  }
  SECTION("Test seq regex negative") {
    auto r1 = context.makeTestRegex(boolTrue);
    auto r2 = context.makePropRegex(a);
    auto seq = context.makeSeqRegex({r1, r2});
    REQUIRE(is_test_only(*seq) == false);
  }
  SECTION("Test union regex positive") {
    auto r1 = context.makeTestRegex(boolTrue);
    auto r2 = context.makeTestRegex(boolFalse);
    auto u = context.makeUnionRegex({r1, r2});
    REQUIRE(is_test_only(*u) == true);
  }
  SECTION("Test union regex negative") {
    auto r1 = context.makeTestRegex(boolTrue);
    auto r2 = context.makePropRegex(a);
    auto u = context.makeUnionRegex({r1, r2});
    REQUIRE(is_test_only(*u) == false);
  }
  SECTION("Test star regex negative") {
    auto r = context.makeTestRegex(boolTrue);
    auto star = context.makeStarRegex(r);
    REQUIRE(is_test_only(*star) == true);
  }
  SECTION("Test star regex negative") {
    auto r = context.makePropRegex(a);
    auto star = context.makeStarRegex(r);
    REQUIRE(is_test_only(*star) == false);
  }
}

} // namespace whitemech::lydia::Test