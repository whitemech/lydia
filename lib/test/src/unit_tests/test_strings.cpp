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
#include <lydia/utils/strings.hpp>

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

TEST_CASE("Test state2bin", "[utils/strings]") {
  SECTION("0 is 0") { REQUIRE(state2bin(0) == "0"); }
  SECTION("1 is 1") { REQUIRE(state2bin(1) == "1"); }
  SECTION("2 is 10") { REQUIRE(state2bin(2) == "10"); }
  SECTION("3 is 11") { REQUIRE(state2bin(3) == "11"); }
  SECTION("4 is 100") { REQUIRE(state2bin(4) == "100"); }
}

TEST_CASE("Test state2bin with fill", "[utils/strings]") {
  SECTION("0,3 is 000") { REQUIRE(state2bin(0, 3) == "000"); }
  SECTION("1,3 is 001") { REQUIRE(state2bin(1, 3) == "001"); }
  SECTION("1,10 is 0000000001") { REQUIRE(state2bin(1, 10) == "0000000001"); }
  SECTION("10,1 is 1010") { REQUIRE(state2bin(10, 1) == "1010"); }
}

} // namespace whitemech::lydia::Test