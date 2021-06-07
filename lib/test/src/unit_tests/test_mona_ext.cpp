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
#include <lydia/dfa/mona_dfa.hpp>
#include <lydia/logger.hpp>
#include <lydia/mona_ext/mona_ext_base.hpp>
#include <test/src/utils/to_dfa.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Test universal project.", "[mona_ext]") {

  auto indices = std::vector<int>({0, 1, 2});
  DFA* a = dfaNext(0);
  DFA* not_a = dfaNext(0, false);
  DFA* b = dfaNext(1);
  DFA* c = dfaNext(2);

  DFA* ab = dfa_concatenate(a, b, 3, indices.data());
  DFA* not_a_c = dfa_concatenate(not_a, c, 3, indices.data());
  DFA* or_ = dfaProduct(ab, not_a_c, dfaOR);

  DFA* projected = dfaMinimize(dfaUniversalProject(or_, 0));
  auto result = mona_dfa(projected, indices.size());
  verify(result, {}, false);
  verify(result, {"100"}, false);
  verify(result, {"010"}, false);
  verify(result, {"001"}, false);
  verify(result, {"011"}, true);
  verify(result, {"111"}, true);
  verify(result, {"011", "000"}, false);
  verify(result, {"111", "000"}, false);

  //  dfaUniversalProject();
}

} // namespace whitemech::lydia::Test