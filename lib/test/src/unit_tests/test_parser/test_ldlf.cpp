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
#include <lydia/parser/ldlf/driver.hpp>
#include <sstream>
#include <string>

namespace whitemech::lydia::Test {

TEST_CASE("Driver LDLf Boolean Atoms", "[parser][ldlf]") {
  Logger log("test_parser");
  auto context = std::make_shared<AstManager>();
  auto driver = parsers::ldlf::Driver(context);

  auto actualBoolTrue = context->makeLdlfTrue();
  auto actualBoolFalse = context->makeLdlfFalse();

  SECTION("test parsing tt") {
    std::istringstream tt("tt");
    driver.parse(tt);
    auto parsedBoolTrue = driver.result;
    REQUIRE(*parsedBoolTrue == *actualBoolTrue);
  }
  SECTION("test parsing ff") {
    std::istringstream ff("ff");
    driver.parse(ff);
    auto parsedBoolFalse = driver.result;
    REQUIRE(*parsedBoolFalse == *actualBoolFalse);
  }
}

TEST_CASE("Driver LDLfAnd between Boolean atoms", "[parser][ldlf]") {
  auto context = std::make_shared<AstManager>();
  auto driver = parsers::ldlf::Driver(context);
  auto tt = context->makeLdlfTrue();
  auto ff = context->makeLdlfFalse();
  auto actualAnd_true_false = context->makeLdlfAnd(set_formulas({tt, ff}));
  auto actualAnd_false_true = context->makeLdlfAnd(set_formulas({ff, tt}));

  SECTION("test parsing tt && ff") {
    std::istringstream tt_and_ff("tt && ff");
    driver.parse(tt_and_ff);
    auto parsedAnd = driver.result;
    REQUIRE(*parsedAnd == *actualAnd_true_false);
  }
  SECTION("test parsing tt & ff") {
    std::istringstream tt_and_ff("tt & ff");
    driver.parse(tt_and_ff);
    auto parsedAnd = driver.result;
    REQUIRE(*parsedAnd == *actualAnd_true_false);
  }
  SECTION("test parsing ff && tt") {
    std::istringstream ff_and_tt("ff && tt");
    driver.parse(ff_and_tt);
    auto parsedAnd = driver.result;
    REQUIRE(*parsedAnd == *actualAnd_false_true);
  }
  SECTION("test parsing (ff && tt)") {
    std::istringstream par_ff_and_tt_par("(ff && tt)");
    driver.parse(par_ff_and_tt_par);
    auto parsedAnd = driver.result;
    REQUIRE(*parsedAnd == *actualAnd_false_true);
  }
  SECTION("test parsing tt & ff & tt") {
    auto and_ff_tt = context->makeLdlfAnd(set_formulas({ff, tt}));
    auto actualAnd_tt_ff_tt =
        context->makeLdlfAnd(set_formulas({and_ff_tt, tt}));
    std::istringstream tt_and_ff_and_tt("tt & ff & tt");
    driver.parse(tt_and_ff_and_tt);
    auto parsedAnd = driver.result;
    REQUIRE(*parsedAnd == *actualAnd_tt_ff_tt);
  }
  SECTION("test parsing (tt & ff) & tt") {
    auto and_tt_ff = context->makeLdlfAnd(set_formulas({tt, ff}));
    auto actualAnd_tt_ff_tt =
        context->makeLdlfAnd(set_formulas({and_tt_ff, tt}));
    std::istringstream tt_and_ff_and_tt("(tt & ff) & tt");
    driver.parse(tt_and_ff_and_tt);
    auto parsedAnd = driver.result;
    REQUIRE(*parsedAnd == *actualAnd_tt_ff_tt);
  }
  SECTION("test parsing tt & (ff & tt)") {
    auto and_ff_tt = context->makeLdlfAnd(set_formulas({ff, tt}));
    auto actualAnd_tt_ff_tt =
        context->makeLdlfAnd(set_formulas({tt, and_ff_tt}));
    std::istringstream tt_and_ff_and_tt("tt & (ff & tt)");
    driver.parse(tt_and_ff_and_tt);
    auto parsedAnd = driver.result;
    REQUIRE(*parsedAnd == *actualAnd_tt_ff_tt);
  }
}

TEST_CASE("Driver LDLfOr between Boolean atoms", "[parser][ldlf]") {
  auto context = std::make_shared<AstManager>();
  auto driver = parsers::ldlf::Driver(context);
  auto tt = context->makeLdlfTrue();
  auto ff = context->makeLdlfFalse();
  auto actualOr_true_false = context->makeLdlfOr(set_formulas({tt, ff}));
  auto actualOr_false_true = context->makeLdlfOr(set_formulas({ff, tt}));

  SECTION("test parsing tt || ff") {
    std::istringstream tt_or_ff("tt || ff");
    driver.parse(tt_or_ff);
    auto parsedOr = driver.result;
    REQUIRE(*parsedOr == *actualOr_true_false);
  }
  SECTION("test parsing tt | ff") {
    std::istringstream tt_or_ff("tt | ff");
    driver.parse(tt_or_ff);
    auto parsedOr = driver.result;
    REQUIRE(*parsedOr == *actualOr_true_false);
  }
  SECTION("test parsing ff || tt") {
    std::istringstream ff_or_tt("ff || tt");
    driver.parse(ff_or_tt);
    auto parsedOr = driver.result;
    REQUIRE(*parsedOr == *actualOr_false_true);
  }
  SECTION("test parsing (ff || tt)") {
    std::istringstream par_ff_or_tt_par("(ff || tt)");
    driver.parse(par_ff_or_tt_par);
    auto parsedOr = driver.result;
    REQUIRE(*parsedOr == *actualOr_false_true);
  }
  SECTION("test parsing tt | ff | tt") {
    auto or_ff_tt = context->makeLdlfOr(set_formulas({ff, tt}));
    auto actualOr_tt_ff_tt = context->makeLdlfOr(set_formulas({or_ff_tt, tt}));
    std::istringstream tt_or_ff_or_tt("tt | ff | tt");
    driver.parse(tt_or_ff_or_tt);
    auto parsedAnd = driver.result;
    REQUIRE(*parsedAnd == *actualOr_tt_ff_tt);
  }
}

TEST_CASE("Driver LDLfNot", "[parser][ldlf]") {
  auto context = std::make_shared<AstManager>();
  auto driver = parsers::ldlf::Driver(context);
  auto tt = context->makeLdlfTrue();
  auto ff = context->makeLdlfFalse();
  auto actualNot_true = context->makeLdlfNot(tt);
  auto actualNot_false = context->makeLdlfNot(ff);

  auto actualAnd_true_false = context->makeLdlfAnd(set_formulas({tt, ff}));
  auto actualNot_and = context->makeLdlfNot(actualAnd_true_false);

  auto actualOr_true_false = context->makeLdlfOr(set_formulas({tt, ff}));
  auto actualNot_or = context->makeLdlfNot(actualOr_true_false);

  SECTION("test parsing ~tt") {
    std::istringstream not_tt("~tt");
    driver.parse(not_tt);
    auto parsedNot_tt = driver.result;
    REQUIRE(*parsedNot_tt == *actualNot_true);
  }
  SECTION("test parsing ~(tt)") {
    std::istringstream not_tt_par("~(tt)");
    driver.parse(not_tt_par);
    auto parsedNot_tt = driver.result;
    REQUIRE(*parsedNot_tt == *actualNot_true);
  }
  SECTION("test parsing ~ff") {
    std::istringstream not_ff("~ff");
    driver.parse(not_ff);
    auto parsedNot_ff = driver.result;
    REQUIRE(*parsedNot_ff == *actualNot_false);
  }
  SECTION("test parsing ~(ff)") {
    std::istringstream not_ff_par("~(ff)");
    driver.parse(not_ff_par);
    auto parsedNot_ff_par = driver.result;
    REQUIRE(*parsedNot_ff_par == *actualNot_false);
  }
  SECTION("test parsing !(tt & ff)") {
    std::istringstream not_tt_and_ff("!(tt & ff)");
    driver.parse(not_tt_and_ff);
    auto parsedNot_tt_and_ff = driver.result;
    REQUIRE(*parsedNot_tt_and_ff == *actualNot_and);
  }
  SECTION("test parsing ~(ff || tt)") {
    std::istringstream par_ff_and_tt_par("~(ff || tt)");
    driver.parse(par_ff_and_tt_par);
    auto parsedNot_ff_or_tt = driver.result;
    REQUIRE(*parsedNot_ff_or_tt == *actualNot_or);
  }
}

TEST_CASE("Driver LDLfTemporal", "[parser][ldlf]") {
  auto context = std::make_shared<AstManager>();
  auto driver = parsers::ldlf::Driver(context);

  auto ptr_prop_re_true = context->makePropRegex(context->makeTrue());
  auto ptr_prop_re_false = context->makePropRegex(context->makeFalse());
  auto ptr_prop_re_a = context->makePropRegex(context->makePropAtom("a"));
  auto ptr_prop_re_b = context->makePropRegex(context->makePropAtom("b"));
  auto ptr_prop_re_c = context->makePropRegex(context->makePropAtom("c"));

  auto ptr_seq_re_ab =
      context->makeSeqRegex(vec_regex({ptr_prop_re_a, ptr_prop_re_b}));
  auto ptr_seq_re =
      context->makeSeqRegex(vec_regex({ptr_seq_re_ab, ptr_prop_re_c}));
  auto ptr_union_re =
      context->makeUnionRegex(set_regex({ptr_prop_re_a, ptr_prop_re_b}));
  auto ptr_star_re = context->makeStarRegex(ptr_prop_re_c);
  auto ptr_test_re = context->makeTestRegex(context->makeLdlfFalse());

  auto ptr_tt = context->makeLdlfTrue();
  auto ptr_ff = context->makeLdlfFalse();
  auto ptr_end = context->makeLdlfBox(ptr_prop_re_true, ptr_ff);
  auto ptr_last = context->makeLdlfDiamond(ptr_prop_re_true, ptr_end);

  auto actualDiamond_prop_re_tt =
      context->makeLdlfDiamond(ptr_prop_re_a, ptr_tt);
  auto actualDiamond_seq_re_tt = context->makeLdlfDiamond(ptr_seq_re, ptr_tt);
  auto actualDiamond_union_re_tt =
      context->makeLdlfDiamond(ptr_union_re, ptr_tt);
  auto actualDiamond_star_re_tt = context->makeLdlfDiamond(ptr_star_re, ptr_tt);
  auto actualDiamond_test_re_tt = context->makeLdlfDiamond(ptr_test_re, ptr_tt);

  auto actualBox_prop_re_tt = context->makeLdlfBox(ptr_prop_re_a, ptr_tt);
  auto actualBox_seq_re_tt = context->makeLdlfBox(ptr_seq_re, ptr_tt);
  auto actualBox_union_re_tt = context->makeLdlfBox(ptr_union_re, ptr_tt);
  auto actualBox_star_re_tt = context->makeLdlfBox(ptr_star_re, ptr_tt);
  auto actualBox_test_re_tt = context->makeLdlfBox(ptr_test_re, ptr_tt);

  auto actualDiamond_a_end = context->makeLdlfDiamond(ptr_prop_re_a, ptr_end);
  auto actualDiamond_a_last = context->makeLdlfDiamond(ptr_prop_re_a, ptr_last);
  auto actualBox_a_end = context->makeLdlfBox(ptr_prop_re_a, ptr_end);
  auto actualBox_a_last = context->makeLdlfBox(ptr_prop_re_a, ptr_last);

  SECTION("test parsing <a>tt") {
    std::istringstream a_tt("<a>tt");
    driver.parse(a_tt);
    auto parsedDiamond = driver.result;
    REQUIRE(*parsedDiamond == *actualDiamond_prop_re_tt);
  }
  SECTION("test parsing <false>tt") {
    std::istringstream a_tt("<false>tt");
    driver.parse(a_tt);
    auto parsedDiamond = driver.result;
    REQUIRE(*dynamic_cast<const LDLfDiamond&>(*parsedDiamond).get_regex() ==
            *ptr_prop_re_false);
  }
  SECTION("test parsing <true>tt") {
    std::istringstream a_tt("<true>tt");
    driver.parse(a_tt);
    auto parsedDiamond = driver.result;
    REQUIRE(*dynamic_cast<const LDLfDiamond&>(*parsedDiamond).get_regex() ==
            *ptr_prop_re_true);
  }
  SECTION("test parsing <a;b;c>tt") {
    std::istringstream abc_tt("<a;b;c>tt");
    driver.parse(abc_tt);
    auto parsedDiamond = driver.result;
    REQUIRE(*parsedDiamond == *actualDiamond_seq_re_tt);
  }
  SECTION("test parsing <a+b>tt") {
    std::istringstream ab_tt("<a+b>tt");
    driver.parse(ab_tt);
    auto parsedDiamond = driver.result;
    REQUIRE(*parsedDiamond == *actualDiamond_union_re_tt);
  }
  SECTION("test parsing <c*>tt") {
    std::istringstream c_tt("<c*>tt");
    driver.parse(c_tt);
    auto parsedDiamond = driver.result;
    REQUIRE(*parsedDiamond == *actualDiamond_star_re_tt);
  }
  SECTION("test parsing <ff?>tt") {
    std::istringstream ff_tt("<ff?>tt");
    driver.parse(ff_tt);
    auto parsedDiamond = driver.result;
    REQUIRE(*parsedDiamond == *actualDiamond_test_re_tt);
  }
  SECTION("test parsing [a]tt") {
    std::istringstream a_tt("[a]tt");
    driver.parse(a_tt);
    auto parsedBox = driver.result;
    REQUIRE(*parsedBox == *actualBox_prop_re_tt);
  }
  SECTION("test parsing [a;b;c]tt") {
    std::istringstream abc_tt("[a;b;c]tt");
    driver.parse(abc_tt);
    auto parsedBox = driver.result;
    REQUIRE(*parsedBox == *actualBox_seq_re_tt);
  }
  SECTION("test parsing [a+b]tt") {
    std::istringstream ab_tt("[a+b]tt");
    driver.parse(ab_tt);
    auto parsedBox = driver.result;
    REQUIRE(*parsedBox == *actualBox_union_re_tt);
  }
  SECTION("test parsing [c*]tt") {
    std::istringstream c_tt("[c*]tt");
    driver.parse(c_tt);
    auto parsedBox = driver.result;
    REQUIRE(*parsedBox == *actualBox_star_re_tt);
  }
  SECTION("test parsing [ff?]tt") {
    std::istringstream ff_tt("[ff?]tt");
    driver.parse(ff_tt);
    auto parsedBox = driver.result;
    REQUIRE(*parsedBox == *actualBox_test_re_tt);
  }
  SECTION("test parsing [a]end") {
    std::istringstream a_end("[a]end");
    driver.parse(a_end);
    auto parsedBox = driver.result;
    REQUIRE(*parsedBox == *actualBox_a_end);
  }
  SECTION("test parsing [a]last") {
    std::istringstream a_last("[a]last");
    driver.parse(a_last);
    auto parsedBox = driver.result;
    REQUIRE(*parsedBox == *actualBox_a_last);
  }
}

TEST_CASE("Driver LDLf Implication", "[parser][ldlf]") {
  auto context = std::make_shared<AstManager>();
  auto driver = parsers::ldlf::Driver(context);
  auto a_tt = context->makeLdlfDiamond(
      context->makePropRegex(context->makePropAtom("a")),
      context->makeLdlfTrue());
  auto b_tt = context->makeLdlfDiamond(
      context->makePropRegex(context->makePropAtom("b")),
      context->makeLdlfTrue());

  SECTION("test parsing (<a>tt -> <b>tt)") {
    std::istringstream f("<a>tt -> <b>tt");
    driver.parse(f);
    auto actual_f = driver.result;
    auto expected_f =
        context->makeLdlfOr(set_formulas{context->makeLdlfNot(a_tt), b_tt});
    REQUIRE(*actual_f == *expected_f);
  }
  SECTION("test parsing (<a>tt -> <a>tt)") {
    std::istringstream f("<a>tt -> <a>tt");
    driver.parse(f);
    auto actual_f = driver.result;
    auto expected_f =
        context->makeLdlfOr(set_formulas{context->makeLdlfNot(a_tt), a_tt});
    REQUIRE(*actual_f == *expected_f);
  }
}

TEST_CASE("Driver LDLf Equivalence", "[parser][ldlf]") {
  auto context = std::make_shared<AstManager>();
  auto driver = parsers::ldlf::Driver(context);
  auto a_tt = context->makeLdlfDiamond(
      context->makePropRegex(context->makePropAtom("a")),
      context->makeLdlfTrue());
  auto b_tt = context->makeLdlfDiamond(
      context->makePropRegex(context->makePropAtom("b")),
      context->makeLdlfTrue());
  auto not_a_tt = context->makeLdlfNot(a_tt);
  auto not_b_tt = context->makeLdlfNot(b_tt);

  SECTION("test parsing (<a>tt <-> <b>tt)") {
    std::istringstream f("<a>tt <-> <b>tt");
    driver.parse(f);
    auto actual_f = driver.result;
    auto left_to_right = context->makeLdlfOr(set_formulas{not_a_tt, b_tt});
    auto right_to_left = context->makeLdlfOr(set_formulas{not_b_tt, a_tt});
    auto expected_f =
        context->makeLdlfAnd(set_formulas{left_to_right, right_to_left});
    REQUIRE(*actual_f == *expected_f);
  }
  SECTION("test parsing (<a>tt <-> <a>tt)") {
    std::istringstream f("<a>tt <-> <a>tt");
    driver.parse(f);
    auto actual_f = driver.result;
    auto left_to_right = context->makeLdlfOr(set_formulas{not_a_tt, a_tt});
    auto right_to_left = context->makeLdlfOr(set_formulas{not_a_tt, a_tt});
    auto expected_f =
        context->makeLdlfAnd(set_formulas{left_to_right, right_to_left});
    REQUIRE(*actual_f == *expected_f);
  }
}

} // namespace whitemech::lydia::Test
