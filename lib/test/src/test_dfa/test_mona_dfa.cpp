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

#include <catch.hpp>
#include <lydia/dfa/mona_dfa.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Test MONA DFA", "[dfa/mona_dfa]") {
  bdd_init();
  dfaSetup(2, 0, nullptr);
  /* boolvar */
  dfaAllocExceptions(0);
  dfaStoreState(1);

  /* state 1 */
  dfaAllocExceptions(0);
  dfaStoreState(1);

  DFA *a = dfaBuild("0+");
  dfaPrintGraphviz(a, 2, {});
};
} // namespace whitemech::lydia::Test