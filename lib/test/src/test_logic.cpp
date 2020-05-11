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
#include <catch.hpp>
#include <iostream>
#include <lydia/logger.hpp>
#include <lydia/logic.hpp>
#include <lydia/utils/compare.hpp>

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

TEST_CASE("LDLfAnd", "[logic]") {
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

TEST_CASE("LDLf simple logical operation", "[logic]") {
  auto tt = boolean(true);
  auto ff = boolean(false);

  SECTION("tt & ff = false") {
    auto tt_and_ff = ldlf_logical_and({tt, ff});
    REQUIRE(tt_and_ff == ff);
  }
  SECTION("tt | ff = tt") {
    auto tt_or_ff = ldlf_logical_or({tt, ff});
    REQUIRE(tt_or_ff == tt);
  }

  auto p = prop_atom("p");
  auto q = prop_atom("q");
  auto ptr_diam_p_tt = std::make_shared<LDLfDiamond>(std::make_shared<PropositionalRegExp>(p) , tt);
  auto ptr_diam_q_tt = std::make_shared<LDLfDiamond>(std::make_shared<PropositionalRegExp>(q) , tt);

  SECTION("and(<p>tt) = <p>tt") {
    auto and_diam_p_tt = ldlf_logical_and({ptr_diam_p_tt});
    REQUIRE(and_diam_p_tt == ptr_diam_p_tt);
  }
  SECTION("<p>tt & ~(<p>tt) = ff") {
    auto ptr_not_diam_p_tt = ptr_diam_p_tt->logical_not();
    auto and_diam_p_tt = ldlf_logical_and({ptr_diam_p_tt, ptr_not_diam_p_tt});
    REQUIRE(and_diam_p_tt == ff);
  }
  SECTION("<p>tt | ~(<p>tt) = tt") {
    auto ptr_not_diam_p_tt = ptr_diam_p_tt->logical_not();
    auto or_diam_p_tt = ldlf_logical_or({ptr_diam_p_tt, ptr_not_diam_p_tt});
    REQUIRE(or_diam_p_tt == tt);
  }
  SECTION("~(<p>tt & <q>tt) = ~<p>tt | ~<q>tt") {
    auto ptr_not_diam_p_tt = ptr_diam_p_tt->logical_not();
    auto ptr_not_diam_q_tt = ptr_diam_q_tt->logical_not();
    auto and_pq = ldlf_logical_and({ptr_diam_p_tt, ptr_diam_q_tt});
    auto expected = ldlf_logical_or({ptr_not_diam_p_tt, ptr_not_diam_q_tt});
    REQUIRE(and_pq->logical_not()->is_equal(*expected));
  }
}

TEST_CASE("LDLf logical and", "[logic]") {
  auto tt = boolean(true);
  auto ff = boolean(false);
  auto a = prop_atom("a");
  auto b = prop_atom("b");
  auto c = prop_atom("c");
  auto ptr_a = std::make_shared<PropositionalRegExp>(a);
  auto ptr_b = std::make_shared<PropositionalRegExp>(b);
  auto ptr_c = std::make_shared<PropositionalRegExp>(c);

  auto diam_a = std::make_shared<LDLfDiamond>(ptr_a, tt);
  auto diam_b = std::make_shared<LDLfDiamond>(ptr_b, tt);
  auto box_c = std::make_shared<LDLfBox>(ptr_c, tt);

  SECTION("(<a>tt & <b>tt) & [c]tt == <a>tt & <b>tt & [c]tt") {
    set_formulas ab = set_formulas({diam_a, diam_b});
    auto ptr_and_ab = std::make_shared<LDLfAnd>(ab);
    auto ptr_and_ab_c = std::make_shared<LDLfAnd>(set_formulas({ptr_and_ab, box_c}));
    auto flattened_and = ldlf_logical_and(ptr_and_ab_c->get_container());
    auto exp_flat_and = std::make_shared<LDLfAnd>(set_formulas({diam_a, diam_b, box_c}));
    REQUIRE(*flattened_and == *exp_flat_and);
  }
  SECTION("<a>tt & (<b>tt & [c]tt) == <a>tt & <b>tt & [c]tt") {
    set_formulas bc = set_formulas({diam_b, box_c});
    auto ptr_and_bc = std::make_shared<LDLfAnd>(bc);
    auto ptr_and_a_bc = std::make_shared<LDLfAnd>(set_formulas({diam_a, ptr_and_bc}));
    auto flattened_and = ldlf_logical_and(ptr_and_a_bc->get_container());
    auto exp_flat_and = std::make_shared<LDLfAnd>(set_formulas({diam_a, diam_b, box_c}));
    REQUIRE(*flattened_and == *exp_flat_and);
  }
  SECTION("<a>tt & <b>tt & ff == ff") {
    set_formulas and_a_b_ff = set_formulas({diam_a, diam_b, ff});
    auto ptr_and_a_b_ff = std::make_shared<LDLfAnd>(and_a_b_ff);
    auto flattened_and = ldlf_logical_and(ptr_and_a_b_ff->get_container());
    REQUIRE(*flattened_and == *ff);
  }
  SECTION("<a>tt & tt & [c]tt == <a>tt & [c]tt") {
    set_formulas and_a_tt_c = set_formulas({diam_a, tt, box_c});
    auto ptr_and_a_tt_c = std::make_shared<LDLfAnd>(and_a_tt_c);
    set_formulas exp_and = set_formulas({diam_a, box_c});
    auto exp_flat_and = std::make_shared<LDLfAnd>(exp_and);
    auto flattened_and = ldlf_logical_and(ptr_and_a_tt_c->get_container());
    REQUIRE(*flattened_and == *exp_flat_and);
  }
}

//TEST_CASE("LDLf logical or", "[logic]") {
//  auto ptr_a = std::make_shared<PropositionalAtom>("a");
//  auto ptr_b = std::make_shared<PropositionalAtom>("b");
//  auto ptr_c = std::make_shared<PropositionalAtom>("c");
//  auto ptr_true = std::make_shared<PropositionalTrue>();
//  auto ptr_false = std::make_shared<PropositionalFalse>();
//
//  set_prop_formulas a_b = set_prop_formulas({ptr_a, ptr_b});
//  auto ptr_or_a_b = std::make_shared<PropositionalOr>(a_b);
//  set_prop_formulas b_c = set_prop_formulas({ptr_b, ptr_c});
//  auto ptr_or_b_c = std::make_shared<PropositionalOr>(b_c);
//
//  SECTION("(a | b) | c == a | b | c") {
//    set_prop_formulas or_ab_c = set_prop_formulas({ptr_or_a_b, ptr_c});
//    auto ptr_or_ab_c = std::make_shared<PropositionalOr>(or_ab_c);
//    set_prop_formulas exp_or = set_prop_formulas({ptr_a, ptr_b, ptr_c});
//    auto exp_flat_or = std::make_shared<PropositionalOr>(exp_or);
//    auto flattened_or = logical_or(ptr_or_ab_c->get_container());
//    REQUIRE(*flattened_or == *exp_flat_or);
//  }
//  SECTION("a | (b | c) == a | b | c") {
//    set_prop_formulas or_a_bc = set_prop_formulas({ptr_a, ptr_or_b_c});
//    auto ptr_or_a_bc = std::make_shared<PropositionalOr>(or_a_bc);
//    set_prop_formulas exp_or = set_prop_formulas({ptr_a, ptr_b, ptr_c});
//    auto exp_flat_or = std::make_shared<PropositionalOr>(exp_or);
//    auto flattened_or = logical_or(ptr_or_a_bc->get_container());
//    REQUIRE(*flattened_or == *exp_flat_or);
//  }
//  SECTION("a | b | false == a | b") {
//    set_prop_formulas or_a_b_false =
//        set_prop_formulas({ptr_a, ptr_b, ptr_false});
//    auto ptr_or_a_b_false = std::make_shared<PropositionalOr>(or_a_b_false);
//    set_prop_formulas exp_or = set_prop_formulas({ptr_a, ptr_b});
//    auto exp_flat_or = std::make_shared<PropositionalOr>(exp_or);
//    auto flattened_or = logical_or(ptr_or_a_b_false->get_container());
//    REQUIRE(*flattened_or == *exp_flat_or);
//  }
//  SECTION("a | true | c == true") {
//    set_prop_formulas or_a_true_c = set_prop_formulas({ptr_a, ptr_true, ptr_c});
//    auto ptr_or_a_true_c = std::make_shared<PropositionalOr>(or_a_true_c);
//    auto flattened_or = logical_or(ptr_or_a_true_c->get_container());
//    REQUIRE(*flattened_or == *ptr_true);
//  }
//}

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
  auto diamond_formula_true_tt = std::make_shared<LDLfDiamond>(regex_true, tt);

  auto regex_false = std::make_shared<const PropositionalRegExp>(false_);
  auto diamond_formula_false_tt =
      std::make_shared<LDLfDiamond>(regex_false, tt);

  auto regex_a = std::make_shared<const PropositionalRegExp>(a);
  auto diamond_formula_a_tt = std::make_shared<LDLfDiamond>(regex_a, tt);
  auto regex_b = std::make_shared<const PropositionalRegExp>(b);
  auto diamond_formula_b_tt = std::make_shared<LDLfDiamond>(regex_b, tt);
  auto regex_a_and_b = std::make_shared<const PropositionalRegExp>(a_and_b);
  auto diamond_formula_a_and_b_tt =
      std::make_shared<LDLfDiamond>(regex_a_and_b, tt);

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