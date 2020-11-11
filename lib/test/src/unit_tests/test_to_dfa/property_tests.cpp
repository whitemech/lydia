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
#include "test/src/assets/formulas.hpp"
#include "test/src/utils/to_dfa.hpp"
#include <catch.hpp>
#include <cppitertools/enumerate.hpp>
#include <cppitertools/product.hpp>
#include <spdlog/fmt/bundled/core.h>

namespace whitemech::lydia::Test {

TEST_CASE("Duality", "[to_dfa]") {
  auto strategy_maker = GENERATE(strategies());
  auto mgr_1 = CUDD::Cudd();
  auto mgr_2 = CUDD::Cudd();
  auto strategy_1 = strategy_maker(mgr_1);
  auto strategy_2 = strategy_maker(mgr_2);
  for (const auto &formula : FORMULAS) {
    SECTION("Test duality of " + formula) {
      adfa_ptr automaton_1 = to_dfa_from_formula_string(formula, *strategy_1);
      adfa_ptr automaton_2 =
          to_dfa_from_formula_string("!(" + formula + ")", *strategy_2);
      REQUIRE(compare<5>(*automaton_1, *automaton_2,
                         automaton_1->get_nb_variables(), not_equal));
    }
  }
}

TEST_CASE("Simple theorems", "[to_dfa]") {
  auto strategy = CompositionalStrategy();
  for (const auto &theorem : SIMPLE_THEOREMS) {
    SECTION("Test theorem " + theorem) {
      adfa_ptr automaton = to_dfa_from_formula_string(theorem, strategy);
      auto true_automaton = std::make_shared<const mona_dfa>(
          dfaTrue(), automaton->get_nb_variables());
      print_mona_dfa(std::static_pointer_cast<const mona_dfa>(automaton)->dfa_,
                     theorem, 1);
      REQUIRE(compare<5>(*automaton, *true_automaton,
                         automaton->get_nb_variables(), equal));
      REQUIRE(automaton->get_nb_states() == 1);
    }
  }
}

TEST_CASE("Advanced theorems", "[to_dfa]") {
  Logger log("advanced-theorems");
  auto strategy = CompositionalStrategy();
  for (auto &&[i, e] :
       iter::enumerate(iter::product(ADVANCED_THEOREMS, FORMULAS, FORMULAS))) {
    const auto [theorem, formula_1, formula_2] = e;
    const auto [theorem_str, nb_formulas] = theorem;
    const auto new_theorem = fmt::format(theorem_str, formula_1, formula_2);
    if (i % 1000 == 0) {
      const std::string section_id =
          fmt::format("Iteration {}: test theorem {}", i, new_theorem);
      log.info(section_id);
    }
    adfa_ptr automaton = to_dfa_from_formula_string(new_theorem, strategy);
    auto true_automaton = std::make_shared<const mona_dfa>(
        dfaTrue(), automaton->get_nb_variables());
    REQUIRE(compare<5>(*automaton, *true_automaton,
                       automaton->get_nb_variables(), equal));
    REQUIRE(automaton->get_nb_states() == 1);
  }
}

} // namespace whitemech::lydia::Test