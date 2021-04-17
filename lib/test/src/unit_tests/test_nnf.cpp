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