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
#include <set>
#include <utils/powerset.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("powerset", "[powerset]") {
  auto s = std::set<int>({0, 1, 2});
  std::vector<std::set<int>> result = powerset<int>(s);
  REQUIRE(result[0] == std::set<int>({}));
  REQUIRE(result[1] == std::set<int>({0}));
  REQUIRE(result[2] == std::set<int>({1}));
  REQUIRE(result[3] == std::set<int>({0, 1}));
  REQUIRE(result[4] == std::set<int>({2}));
  REQUIRE(result[5] == std::set<int>({0, 2}));
  REQUIRE(result[6] == std::set<int>({1, 2}));
  REQUIRE(result[7] == std::set<int>({0, 1, 2}));
}

} // namespace whitemech::lydia::Test