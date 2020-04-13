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
#include "nnf.hpp"
#include <iostream>
#include <utils/compare.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Test set formulas comparator", "[nnf]") {
  auto tt = boolean(true);
  auto ff = boolean(false);
  REQUIRE(*ff < *tt);

  auto formulas = set_formulas();
  formulas.insert(tt);
  formulas.insert(ff);

  vec_formulas result;
  for (const auto &formula : formulas) {
    result.push_back(formula);
  }

  REQUIRE(result[0]->is_equal(*ff));
  REQUIRE(result[1]->is_equal(*tt));
}

TEST_CASE("Negative normal form", "[nnf]") {

  //  SECTION("tt") {
  //    auto tt = LDLfBooleanAtom(true);
  //    REQUIRE(tt == *to_nnf(tt));
  //  }
  //
  //  SECTION("ff") {
  //    auto ff = LDLfBooleanAtom(false);
  //    REQUIRE(ff == *to_nnf(ff));
  //  }
  //
  //  SECTION("!tt") {
  //    auto not_ff = LDLfNot(std::make_shared<LDLfBooleanAtom>(false));
  //    REQUIRE(LDLfBooleanAtom(true) == *to_nnf(not_ff));
  //  }

  //  TODO Fix comparator
  SECTION("!(ff & ff)") {
    auto ffs = set_formulas({boolean(false), boolean(false)});
    auto tts = set_formulas({boolean(true), boolean(true)});
    auto not_and = LDLfNot(std::make_shared<LDLfAnd>(ffs));
    auto expected_nnf = std::make_shared<LDLfOr>(tts);
    auto actual_nnf = to_nnf(not_and);
    bool res = expected_nnf->is_equal(*actual_nnf);
    REQUIRE(res);
  }
}

} // namespace whitemech::lydia::Test