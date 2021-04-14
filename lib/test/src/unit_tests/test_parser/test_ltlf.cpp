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
#include <lydia/parser/ltlf/driver.hpp>
#include <sstream>
#include <string>

namespace whitemech::lydia::Test {

// TODO LTLf parser should produce LTLf formulae, not LDLf formulae.
// TEST_CASE("LTLfDriver LTLf Boolean Atoms", "[parser][ltlf]") {
//  Logger log("test_parser");
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//
//  auto actualBoolTrue = context->makeLtlfTrue();
//  auto actualBoolFalse = context->makeLtlfFalse();
//
//  SECTION("test parsing true") {
//    std::istringstream tt("true");
//    driver.parse(tt);
//    auto parsedBoolTrue = driver.result;
//    REQUIRE(*parsedBoolTrue == *actualBoolTrue);
//  }
//  SECTION("test parsing false") {
//    std::istringstream ff("false");
//    driver.parse(ff);
//    auto parsedBoolFalse = driver.result;
//    REQUIRE(*parsedBoolFalse == *actualBoolFalse);
//  }
//}
//
// TEST_CASE("LTLfDriver LTLfAnd between Boolean atoms", "[parser][ltlf]") {
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//  auto tt = context->makeLtlfTrue();
//  auto ff = context->makeLtlfFalse();
//  auto actualAnd_true_false = context->makeLtlfAnd(set_ltlf_formulas({tt,
//  ff})); auto actualAnd_false_true =
//  context->makeLtlfAnd(set_ltlf_formulas({ff, tt}));
//
//  SECTION("test parsing false && true") {
//    std::istringstream ff_and_tt("false && true");
//    driver.parse(ff_and_tt);
//    auto parsedAnd = driver.result;
//    REQUIRE(*parsedAnd == *actualAnd_false_true);
//  }
//  SECTION("test parsing (false && true)") {
//    std::istringstream par_ff_and_tt_par("(false && true)");
//    driver.parse(par_ff_and_tt_par);
//    auto parsedAnd = driver.result;
//    REQUIRE(*parsedAnd == *actualAnd_false_true);
//  }
//  SECTION("test parsing true & false & true") {
//    auto and_ff_tt = context->makeLtlfAnd(set_ltlf_formulas({ff, tt}));
//    auto actualAnd_tt_ff_tt =
//        context->makeLtlfAnd(set_ltlf_formulas({and_ff_tt, tt}));
//    std::istringstream tt_and_ff_and_tt("true && false && true");
//    driver.parse(tt_and_ff_and_tt);
//    auto parsedAnd = driver.result;
//    REQUIRE(*parsedAnd == *actualAnd_tt_ff_tt);
//  }
//  SECTION("test parsing (true & false) & true") {
//    auto and_tt_ff = context->makeLtlfAnd(set_ltlf_formulas({tt, ff}));
//    auto actualAnd_tt_ff_tt =
//        context->makeLtlfAnd(set_ltlf_formulas({and_tt_ff, tt}));
//    std::istringstream tt_and_ff_and_tt("(true & false) & true");
//    driver.parse(tt_and_ff_and_tt);
//    auto parsedAnd = driver.result;
//    REQUIRE(*parsedAnd == *actualAnd_tt_ff_tt);
//  }
//  SECTION("test parsing true & (false & true)") {
//    auto and_ff_tt = context->makeLtlfAnd(set_ltlf_formulas({ff, tt}));
//    auto actualAnd_tt_ff_tt =
//        context->makeLtlfAnd(set_ltlf_formulas({tt, and_ff_tt}));
//    std::istringstream tt_and_ff_and_tt("true & (false & true)");
//    driver.parse(tt_and_ff_and_tt);
//    auto parsedAnd = driver.result;
//    REQUIRE(*parsedAnd == *actualAnd_tt_ff_tt);
//  }
//}
//
// TEST_CASE("LTLfDriver LTLfOr between Boolean atoms", "[parser][ltlf]") {
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//  auto tt = context->makeLtlfTrue();
//  auto ff = context->makeLtlfFalse();
//  auto actualOr_true_false = context->makeLtlfOr(set_ltlf_formulas({tt, ff}));
//  auto actualOr_false_true = context->makeLtlfOr(set_ltlf_formulas({ff, tt}));
//
//  SECTION("test parsing true || false") {
//    std::istringstream tt_or_ff("true || false");
//    driver.parse(tt_or_ff);
//    auto parsedOr = driver.result;
//    REQUIRE(*parsedOr == *actualOr_true_false);
//  }
//  SECTION("test parsing true | false") {
//    std::istringstream tt_or_ff("true | false");
//    driver.parse(tt_or_ff);
//    auto parsedOr = driver.result;
//    REQUIRE(*parsedOr == *actualOr_true_false);
//  }
//  SECTION("test parsing false || true") {
//    std::istringstream ff_or_tt("false || true");
//    driver.parse(ff_or_tt);
//    auto parsedOr = driver.result;
//    REQUIRE(*parsedOr == *actualOr_false_true);
//  }
//  SECTION("test parsing (false || true)") {
//    std::istringstream par_ff_or_tt_par("(false || true)");
//    driver.parse(par_ff_or_tt_par);
//    auto parsedOr = driver.result;
//    REQUIRE(*parsedOr == *actualOr_false_true);
//  }
//  SECTION("test parsing true | false | true") {
//    auto or_ff_tt = context->makeLtlfOr(set_ltlf_formulas({ff, tt}));
//    auto actualOr_tt_ff_tt =
//        context->makeLtlfOr(set_ltlf_formulas({or_ff_tt, tt}));
//    std::istringstream tt_or_ff_or_tt("true | false | true");
//    driver.parse(tt_or_ff_or_tt);
//    auto parsedAnd = driver.result;
//    REQUIRE(*parsedAnd == *actualOr_tt_ff_tt);
//  }
//}
//
// TEST_CASE("LTLfDriver LTLfNot", "[parser][ltlf]") {
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//  auto tt = context->makeLtlfTrue();
//  auto ff = context->makeLtlfFalse();
//  auto actualNot_true = context->makeLtlfNot(tt);
//  auto actualNot_false = context->makeLtlfNot(ff);
//
//  auto actualAnd_true_false = context->makeLtlfAnd(set_ltlf_formulas({tt,
//  ff})); auto actualNot_and = context->makeLtlfNot(actualAnd_true_false);
//
//  auto actualOr_true_false = context->makeLtlfOr(set_ltlf_formulas({tt, ff}));
//  auto actualNot_or = context->makeLtlfNot(actualOr_true_false);
//
//  SECTION("test parsing ~true") {
//    std::istringstream not_tt("~true");
//    driver.parse(not_tt);
//    auto parsedNot_tt = driver.result;
//    REQUIRE(*parsedNot_tt == *actualNot_true);
//  }
//  SECTION("test parsing ~(true)") {
//    std::istringstream not_tt_par("~(true)");
//    driver.parse(not_tt_par);
//    auto parsedNot_tt = driver.result;
//    REQUIRE(*parsedNot_tt == *actualNot_true);
//  }
//  SECTION("test parsing ~false") {
//    std::istringstream not_ff("~false");
//    driver.parse(not_ff);
//    auto parsedNot_ff = driver.result;
//    REQUIRE(*parsedNot_ff == *actualNot_false);
//  }
//  SECTION("test parsing ~(false)") {
//    std::istringstream not_ff_par("~(false)");
//    driver.parse(not_ff_par);
//    auto parsedNot_ff_par = driver.result;
//    REQUIRE(*parsedNot_ff_par == *actualNot_false);
//  }
//  SECTION("test parsing !(true & false)") {
//    std::istringstream not_tt_and_ff("!(true & false)");
//    driver.parse(not_tt_and_ff);
//    auto parsedNot_tt_and_ff = driver.result;
//    REQUIRE(*parsedNot_tt_and_ff == *actualNot_and);
//  }
//  SECTION("test parsing ~(false || true)") {
//    std::istringstream par_ff_and_tt_par("~(false || true)");
//    driver.parse(par_ff_and_tt_par);
//    auto parsedNot_ff_or_tt = driver.result;
//    REQUIRE(*parsedNot_ff_or_tt == *actualNot_or);
//  }
//}
//
// TEST_CASE("LTLfDriver atoms", "[parser][ltlf]") {
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//
//  auto true_ = context->makePropRegex(context->makeTrue());
//  auto a = context->makeLtlfAtom("a");
//  auto b = context->makeLtlfAtom("b");
//  auto c = context->makeLtlfAtom("c");
//
//  SECTION("test parsing 'a'") {
//    std::istringstream stream("a");
//    driver.parse(stream);
//    auto actual_formula = driver.result;
//    auto expected_formula = a;
//    REQUIRE(*actual_formula == *expected_formula);
//  }
//
//  SECTION("test parsing 'a & b'") {
//    std::istringstream stream("a & b");
//    driver.parse(stream);
//    auto actual_formula = driver.result;
//    auto expected_formula = context->makeLtlfAnd(set_ltlf_formulas{a, b});
//    REQUIRE(*actual_formula == *expected_formula);
//  }
//
//  SECTION("test parsing 'a | ~c'") {
//    std::istringstream stream("a | ~c");
//    driver.parse(stream);
//    auto actual_formula = driver.result;
//    auto expected_formula =
//        context->makeLtlfOr(set_ltlf_formulas{a, context->makeLtlfNot(c)});
//    REQUIRE(*actual_formula == *expected_formula);
//  }
//}
//
// TEST_CASE("LTLfDriver LTLfNext", "[parser][ltlf]") {
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//
//  auto a = context->makeLtlfAtom("a");
//
//  SECTION("test parsing X(a)") {
//    std::istringstream stream("X(a)");
//    driver.parse(stream);
//    auto actual_formula = driver.result;
//    auto expected_formula = context->makeLtlfNext(a);
//    REQUIRE(*actual_formula == *expected_formula);
//  }
//}
//
// TEST_CASE("LTLfDriver LTLfWeakNext", "[parser][ltlf]") {
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//
//  auto a = context->makeLtlfAtom("a");
//
//  SECTION("test parsing W(a)") {
//    std::istringstream stream("W(a)");
//    driver.parse(stream);
//    auto actual_formula = driver.result;
//    auto expected_formula = context->makeLtlfWeakNext(a);
//    REQUIRE(*actual_formula == *expected_formula);
//  }
//}
//
// TEST_CASE("LTLfDriver LTLfUntil", "[parser][ltlf]") {
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//
//  auto a = context->makeLtlfAtom("a");
//  auto b = context->makeLtlfAtom("b");
//  auto c = context->makeLtlfAtom("c");
//
//  SECTION("test parsing a U b") {
//    std::istringstream stream("a U b");
//    driver.parse(stream);
//    auto actual_formula = driver.result;
//    auto expected_formula = context->makeLtlfUntil(a, b);
//    REQUIRE(*actual_formula == *expected_formula);
//  }
//
//  SECTION("test parsing (a & c) U (b | ~c)") {
//    std::istringstream stream("(a & c) U (b | ~c)");
//    driver.parse(stream);
//    auto actual_formula = driver.result;
//    auto a_and_c = context->makeLtlfAnd(set_ltlf_formulas{a, c});
//    auto b_or_not_c =
//        context->makeLtlfOr(set_ltlf_formulas{b, context->makeLtlfNot(c)});
//    auto expected_formula = context->makeLtlfUntil(a_and_c, b_or_not_c);
//    REQUIRE(*actual_formula == *expected_formula);
//  }
//}
//
// TEST_CASE("LTLfDriver LTLfRelease", "[parser][ltlf]") {
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//
//  auto a = context->makeLtlfAtom("a");
//  auto b = context->makeLtlfAtom("b");
//  auto c = context->makeLtlfAtom("c");
//
//  SECTION("test parsing a R b") {
//    std::istringstream stream("a R b");
//    driver.parse(stream);
//    auto actual_formula = driver.result;
//    auto expected_formula = context->makeLtlfRelease(a, b);
//    REQUIRE(*actual_formula == *expected_formula);
//  }
//
//  SECTION("test parsing (a & c) R (b | ~c)") {
//    std::istringstream stream("(a & c) R (b | ~c)");
//    driver.parse(stream);
//    auto actual_formula = driver.result;
//    auto a_and_c = context->makeLtlfAnd(set_ltlf_formulas{a, c});
//    auto b_or_not_c =
//        context->makeLtlfOr(set_ltlf_formulas{b, context->makeLtlfNot(c)});
//    auto expected_formula = context->makeLtlfRelease(a_and_c, b_or_not_c);
//    REQUIRE(*actual_formula == *expected_formula);
//  }
//}
//
// TEST_CASE("LTLfDriver LTLfEventually", "[parser][ltlf]") {
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//
//  auto a = context->makeLtlfAtom("a");
//
//  SECTION("test parsing F(a)") {
//    std::istringstream stream("F(a)");
//    driver.parse(stream);
//    auto actual_formula = driver.result;
//    auto expected_formula = context->makeLtlfEventually(a);
//    REQUIRE(*actual_formula == *expected_formula);
//  }
//}
// TEST_CASE("LTLfDriver LTLfAlways", "[parser][ltlf]") {
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//
//  auto a = context->makeLtlfAtom("a");
//
//  SECTION("test parsing F(a)") {
//    std::istringstream stream("G(a)");
//    driver.parse(stream);
//    auto actual_formula = driver.result;
//    auto expected_formula = context->makeLtlfAlways(a);
//    REQUIRE(*actual_formula == *expected_formula);
//  }
//}
//
// TEST_CASE("LTLfDriver LTLf Implication", "[parser][ltlf]") {
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//  auto a = context->makeLtlfAtom("a");
//  auto b = context->makeLtlfAtom("b");
//
//  SECTION("test parsing (a -> b)") {
//    std::istringstream f("a -> b");
//    driver.parse(f);
//    auto actual_f = driver.result;
//    auto expected_f =
//        context->makeLtlfOr(set_ltlf_formulas{context->makeLtlfNot(a), b});
//    REQUIRE(*actual_f == *expected_f);
//  }
//}
//
// TEST_CASE("LTLfDriver LTLf Equivalence", "[parser][ltlf]") {
//  auto context = std::make_shared<AstManager>();
//  auto driver = parsers::ltlf::LTLfDriver(context);
//  auto a = context->makeLtlfAtom("a");
//  auto b = context->makeLtlfAtom("b");
//  auto not_a = context->makeLtlfNot(a);
//  auto not_b = context->makeLtlfNot(b);
//
//  SECTION("test parsing (a <-> b)") {
//    std::istringstream f("a <-> b");
//    driver.parse(f);
//    auto actual_f = driver.result;
//    auto left_to_right = context->makeLtlfOr(set_ltlf_formulas{not_a, b});
//    auto right_to_left = context->makeLtlfOr(set_ltlf_formulas{not_b, a});
//    auto expected_f =
//        context->makeLtlfAnd(set_ltlf_formulas{left_to_right, right_to_left});
//    REQUIRE(*actual_f == *expected_f);
//  }
//}

} // namespace whitemech::lydia::Test
