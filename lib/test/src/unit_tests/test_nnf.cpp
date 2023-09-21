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
#include <lydia/logic/nnf.hpp>
#include <lydia/utils/compare.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Test NNF of tt") {
  auto context = AstManager{};
  auto f = context.makeLtlfTrue();
  auto actual_formula = to_nnf(*f);
  REQUIRE(f == actual_formula);
}

TEST_CASE("Test NNF of ff") {
  auto context = AstManager{};
  auto f = context.makeLtlfFalse();
  auto actual_formula = to_nnf(*f);
  REQUIRE(f == actual_formula);
}

TEST_CASE("Test NNF of !tt") {
  auto context = AstManager{};
  auto f = context.makeLtlfNot(context.makeLtlfTrue());
  auto actual_formula = to_nnf(*f);
  auto expected_formula = context.makeLtlfFalse();
  REQUIRE(expected_formula == actual_formula);
}

TEST_CASE("Test NNF of !ff") {
  auto context = AstManager{};
  auto f = context.makeLtlfNot(context.makeLtlfFalse());
  auto actual_formula = to_nnf(*f);
  auto expected_formula = context.makeLtlfTrue();
  REQUIRE(expected_formula == actual_formula);
}

TEST_CASE("Test NNF of atomic") {
  auto context = AstManager{};
  auto a = context.makeLtlfAtom("a");
  auto actual_formula = to_nnf(*a);
  REQUIRE(a == actual_formula);
}

TEST_CASE("Test NNF of negated atomic") {
  auto context = AstManager{};
  auto a = context.makeLtlfAtom("a");
  auto not_a = context.makeLtlfNot(a);
  auto actual_formula = to_nnf(*not_a);
  REQUIRE(not_a == actual_formula);
}

TEST_CASE("Test NNF of doubly negated atomic") {
  auto context = AstManager{};
  auto a = context.makeLtlfAtom("!!a");
  auto not_not_a = context.makeLtlfNot(context.makeLtlfNot(a));
  auto actual_formula = to_nnf(*not_not_a);
  REQUIRE(a == actual_formula);
}

TEST_CASE("Test NNF of logical negation of atomic") {
  // ~a = !a | end
  auto context = AstManager{};
  auto a = context.makeLtlfAtom("a");
  auto not_a = context.makeLtlfNot(a);
  auto logical_not_a = context.makeLtlfNot(a);
  auto actual_formula = to_nnf(*logical_not_a);
  REQUIRE(logical_not_a == actual_formula);
}

TEST_CASE("Test NNF of next") {
  // ~X[!]a = X(!a | end)
  auto context = AstManager{};
  auto a = context.makeLtlfAtom("a");
  auto not_a = context.makeLtlfNot(a);
  auto end = context.makeLtlfEnd();
  auto not_a_or_end = context.makeLtlfOr(set_ltlf_formulas{not_a, end});
  auto next_a = context.makeLtlfNext(a);
  auto not_next_a = context.makeLtlfNot(next_a);
  auto weak_next_not_a = context.makeLtlfWeakNext(not_a_or_end);
  auto actual = to_nnf(*not_next_a);
  const auto& expected = weak_next_not_a;
  REQUIRE(actual == expected);
}

TEST_CASE("Test NNF of weak next") {
  // ~X a = X[!](!a | end)
  auto context = AstManager{};
  auto a = context.makeLtlfAtom("a");
  auto not_a = context.makeLtlfNot(a);
  auto end = context.makeLtlfEnd();
  auto weak_next_a = context.makeLtlfWeakNext(a);
  auto not_a_or_end = context.makeLtlfOr(set_ltlf_formulas{not_a, end});
  auto next_not_a = context.makeLtlfNext(not_a_or_end);
  auto not_weak_next_a = context.makeLtlfNot(weak_next_a);
  auto actual = to_nnf(*not_weak_next_a);
  const auto& expected = next_not_a;
  REQUIRE(actual == expected);
}

TEST_CASE("Test NNF of until") {
  // ~(a U b) = ((!a | end) R (!b | end))
  auto context = AstManager{};
  auto a = context.makeLtlfAtom("a");
  auto b = context.makeLtlfAtom("b");
  auto not_a = context.makeLtlfNot(a);
  auto not_b = context.makeLtlfNot(b);
  auto end = context.makeLtlfEnd();
  auto not_a_or_end = context.makeLtlfOr(set_ltlf_formulas{not_a, end});
  auto not_b_or_end = context.makeLtlfOr(set_ltlf_formulas{not_b, end});
  auto a_until_b = context.makeLtlfUntil(a, b);
  auto not_a_until_b = context.makeLtlfNot(a_until_b);
  auto not_a_release_not_b =
      context.makeLtlfRelease(not_a_or_end, not_b_or_end);
  auto actual = to_nnf(*not_a_until_b);
  const auto& expected = not_a_release_not_b;
  REQUIRE(actual == expected);
}

TEST_CASE("Test NNF of release") {
  // ~(a R b) = ((!a | end) U (!b | end))
  auto context = AstManager{};
  auto a = context.makeLtlfAtom("a");
  auto b = context.makeLtlfAtom("b");
  auto not_a = context.makeLtlfNot(a);
  auto not_b = context.makeLtlfNot(b);
  auto end = context.makeLtlfEnd();
  auto not_a_or_end = context.makeLtlfOr(set_ltlf_formulas{not_a, end});
  auto not_b_or_end = context.makeLtlfOr(set_ltlf_formulas{not_b, end});
  auto a_release_b = context.makeLtlfRelease(a, b);
  auto not_a_release_b = context.makeLtlfNot(a_release_b);
  auto not_a_until_not_b = context.makeLtlfUntil(not_a_or_end, not_b_or_end);
  auto actual = to_nnf(*not_a_release_b);
  const auto& expected = not_a_until_not_b;
  REQUIRE(actual == expected);
}

TEST_CASE("Test NNF of eventually") {
  // ~Fa = G(!a | end)
  auto context = AstManager{};
  auto a = context.makeLtlfAtom("a");
  auto not_a = context.makeLtlfNot(a);
  auto end = context.makeLtlfEnd();
  auto not_a_or_end = context.makeLtlfOr(set_ltlf_formulas{not_a, end});
  auto eventually_a = context.makeLtlfEventually(a);
  auto not_eventually_a = context.makeLtlfNot(eventually_a);
  auto always_not_a = context.makeLtlfAlways(not_a_or_end);
  auto actual = to_nnf(*not_eventually_a);
  const auto& expected = always_not_a;
  REQUIRE(actual == expected);
}

TEST_CASE("Test NNF of always") {
  // ~Ga = F(!a | end)
  auto context = AstManager{};
  auto a = context.makeLtlfAtom("a");
  auto not_a = context.makeLtlfNot(a);
  auto end = context.makeLtlfEnd();
  auto not_a_or_end = context.makeLtlfOr(set_ltlf_formulas{not_a, end});
  auto always_a = context.makeLtlfAlways(a);
  auto not_always_a = context.makeLtlfNot(always_a);
  auto eventually_not_a = context.makeLtlfEventually(not_a_or_end);
  auto actual = to_nnf(*not_always_a);
  const auto& expected = eventually_not_a;
  REQUIRE(actual == expected);
}

TEST_CASE("Negative normal form", "[nnf]") {
  auto context = AstManager{};
  SECTION("tt") {
    auto tt = context.makeLdlfTrue();
    REQUIRE(*tt == *to_nnf(*tt));
  }

  SECTION("ff") {
    auto ff = context.makeLdlfFalse();
    REQUIRE(*ff == *to_nnf(*ff));
  }

  SECTION("!tt") {
    auto tt = context.makeLdlfTrue();
    auto ff = context.makeLdlfFalse();
    auto not_ff = context.makeLdlfNot(ff);
    REQUIRE(*tt == *to_nnf(*not_ff));
  }

  SECTION("!(ff & tt)") {
    auto tt = context.makeLdlfTrue();
    auto ff = context.makeLdlfFalse();
    auto not_and =
        context.makeLdlfNot(context.makeLdlfAnd(set_formulas({ff, tt})));
    auto tt_or = tt;
    auto ff_or = ff;
    auto expected_nnf = context.makeLdlfOr(set_formulas({tt_or, ff_or}));
    auto actual_nnf = to_nnf(*not_and);
    bool res = expected_nnf->is_equal(*actual_nnf);
    REQUIRE(res);
  }
}

} // namespace whitemech::lydia::Test