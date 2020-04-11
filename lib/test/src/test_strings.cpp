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
#include "utils/strings.hpp"

namespace whitemech::lydia::Test {

TEST_CASE("test string utils", "[utils/strings]") {

  SECTION("test to_upper_case") {
    std::string original = "hello, world!";
    std::string expected = "HELLO, WORLD!";
    std::string actual = to_upper_case(original);
    REQUIRE(actual == expected);
    REQUIRE(original == "hello, world!");
  }

  SECTION("test split") {
    std::string original = "apples, oranges, bananas, pineapples";
    std::vector<std::string> expected = {"apples", "oranges", "bananas",
                                         "pineapples"};
    auto actual = split(original, ", ");
    REQUIRE(actual == expected);
  }
}

} // namespace whitemech::lydia::Test