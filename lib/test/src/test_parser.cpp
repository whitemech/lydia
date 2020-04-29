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
#include "parser/driver.hpp"
#include <sstream>
#include <string>

namespace whitemech::lydia::Test {

TEST_CASE("Driver LDLf Boolean Atoms", "[parser]") {
  Logger log("test_parser");
  auto driver = Driver();

  auto actualBoolTrue = std::make_shared<LDLfBooleanAtom>(true);
  auto actualBoolFalse = std::make_shared<LDLfBooleanAtom>(false);

  SECTION("test parsing tt") {
    std::istringstream tt("tt");
    driver.parse(tt);
    auto parsedBoolTrue = driver.result;
    REQUIRE(*parsedBoolTrue == *actualBoolTrue);
  }
  SECTION("test parsing TT") {
    std::istringstream tt_maiusc("TT");
    driver.parse(tt_maiusc);
    auto parsedBoolTrue_MAIUSC = driver.result;
    REQUIRE(*parsedBoolTrue_MAIUSC == *actualBoolTrue);
  }
  SECTION("test parsing ff") {
    std::istringstream ff("ff");
    driver.parse(ff);
    auto parsedBoolFalse = driver.result;
    REQUIRE(*parsedBoolFalse == *actualBoolFalse);
  }
  SECTION("test parsing FF") {
    std::istringstream ff_maiusc("FF");
    driver.parse(ff_maiusc);
    auto parsedBoolFalse_MAIUSC = driver.result;
    REQUIRE(*parsedBoolFalse_MAIUSC == *actualBoolFalse);
  }
}

TEST_CASE("Driver LDLfAnd between Boolean atoms", "[parser]") {
  auto driver = Driver();
  auto actualAnd_true_false =
      std::make_shared<LDLfAnd>(set_formulas({boolean(true), boolean(false)}));
  auto actualAnd_false_true =
      std::make_shared<LDLfAnd>(set_formulas({boolean(false), boolean(true)}));

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
}

TEST_CASE("Driver LDLfOr between Boolean atoms", "[parser]") {
  auto driver = Driver();
  auto actualOr_true_false =
      std::make_shared<LDLfOr>(set_formulas({boolean(true), boolean(false)}));
  auto actualOr_false_true =
      std::make_shared<LDLfOr>(set_formulas({boolean(false), boolean(true)}));

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
}

TEST_CASE("Driver LDLfNot", "[parser]") {
  auto driver = Driver();
  auto actualNot_true = std::make_shared<LDLfNot>(boolean(true));
  auto actualNot_false = std::make_shared<LDLfNot>(boolean(false));

  auto actualAnd_true_false =
      std::make_shared<LDLfAnd>(set_formulas({boolean(true), boolean(false)}));
  auto actualNot_and = std::make_shared<LDLfNot>(actualAnd_true_false);

  auto actualOr_true_false =
      std::make_shared<LDLfOr>(set_formulas({boolean(true), boolean(false)}));
  auto actualNot_or = std::make_shared<LDLfNot>(actualOr_true_false);

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

} // namespace whitemech::lydia::Test
