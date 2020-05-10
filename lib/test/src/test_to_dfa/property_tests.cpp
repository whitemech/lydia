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
#include "../data/formulas.hpp"
#include "../utils/to_dfa.hpp"
#include <catch.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Duality", "[to_dfa]") {
  CUDD::Cudd mgr1;
  CUDD::Cudd mgr2;
  for (const auto &formula : FORMULAS) {
    SECTION("Test duality of " + formula) {
      dfa_ptr automaton_1 = to_dfa_from_formula_string(formula, mgr1);
      dfa_ptr automaton_2 =
          to_dfa_from_formula_string("!(" + formula + ")", mgr2);
      REQUIRE(compare<5>(*automaton_1, *automaton_2, automaton_1->nb_variables,
                         not_equal));
    }
  }
}
} // namespace whitemech::lydia::Test