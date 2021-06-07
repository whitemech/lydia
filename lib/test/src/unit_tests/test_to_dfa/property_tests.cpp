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
#include "test/src/assets/formulas.hpp"
#include "test/src/utils/to_dfa.hpp"
#include <catch.hpp>
#include <cppitertools/enumerate.hpp>
#include <cppitertools/product.hpp>
#include <spdlog/fmt/bundled/core.h>

namespace whitemech::lydia::Test {

TEST_CASE("Duality", "[to_dfa][property_tests][duality]") {
  auto strategy_1 = CompositionalStrategy();
  auto strategy_2 = CompositionalStrategy();
  for (const auto& [i, formula] : iter::enumerate(FORMULAS)) {
    SECTION(fmt::format("Test duality of formula {} '{}'", i, formula)) {
      auto negated_formula = "!(" + formula + ")";
      adfa_ptr automaton_1 = to_dfa_from_formula_string(formula, strategy_1);
      adfa_ptr automaton_2 =
          to_dfa_from_formula_string(negated_formula, strategy_2);
      REQUIRE(compare<5>(*automaton_1, *automaton_2,
                         automaton_1->get_nb_variables(), not_equal));
    }
  }
}

TEST_CASE("Simple theorems", "[to_dfa][property_tests][simple_theorems]") {
  auto strategy = CompositionalStrategy();
  for (const auto& theorem : SIMPLE_THEOREMS) {
    SECTION("Test theorem " + theorem) {
      adfa_ptr automaton = to_dfa_from_formula_string(theorem, strategy);
      auto true_automaton = std::make_shared<const mona_dfa>(
          dfaTrue(), automaton->get_nb_variables());
      REQUIRE(automaton->get_nb_states() == 1);
      REQUIRE(automaton->is_final(automaton->get_initial_state()));
      auto symbol = interpretation(automaton->get_nb_variables());
      auto successor =
          automaton->get_successor(automaton->get_initial_state(), symbol);
      REQUIRE(successor == automaton->get_initial_state());
    }
  }
}

TEST_CASE("Advanced theorems", "[to_dfa][property_tests][advanced_theorems]") {
  Logger log("advanced-theorems");
  int i = 0;
  for (auto&& [i, e] :
       iter::enumerate(iter::product(ADVANCED_THEOREMS, FORMULAS, FORMULAS))) {
    const auto [theorem, formula_1, formula_2] = e;
    const auto [theorem_str, nb_formulas] = theorem;
    const auto new_theorem = fmt::format(theorem_str, formula_1, formula_2);
    const std::string section_id =
        fmt::format("Iteration {}: test theorem {}", i, new_theorem);
    log.info(section_id);
    auto strategy = CompositionalStrategy();
    adfa_ptr automaton = to_dfa_from_formula_string(new_theorem, strategy);
    REQUIRE(automaton->get_nb_states() == 1);
    REQUIRE(automaton->is_final(automaton->get_initial_state()));
    auto symbol = interpretation(automaton->get_nb_variables());
    auto successor =
        automaton->get_successor(automaton->get_initial_state(), symbol);
    REQUIRE(successor == automaton->get_initial_state());
  }
  log.info(fmt::format("Processed {} formulas.", i));
}

} // namespace whitemech::lydia::Test