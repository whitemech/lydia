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
#include <translate.hpp>
#include <utils/dfa_transform.hpp>
#include <utils/print.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Set of DFA states", "[translate]") {
  auto a = DFAState(set_nfa_states{});
  auto b = DFAState(set_formulas{std::make_shared<LDLfBooleanAtom>(true)});
  auto c = DFAState(set_nfa_states{});

  REQUIRE(a == c);
  REQUIRE(c == a);
  REQUIRE(!(a == b));
  REQUIRE(a < b);
}

TEST_CASE("Translate !(ff & tt)", "[translate]") {
  auto args = set_formulas({boolean(false), boolean(true)});
  auto ff_and_tt = std::make_shared<LDLfAnd>(args);
  auto not_and = LDLfNot(ff_and_tt);
  auto formula_name = to_string(*ff_and_tt);
  // TODO complete.
  auto *my_dfa = to_dfa(not_and);

  // print the DFA
  dfa_to_graphviz(*my_dfa, "translate_output_" + formula_name + ".svg", "svg");

  auto false_ = interpretation{};
  auto empty_trace = trace{};
  auto trace_one = trace{false_};
  auto trace_two = trace{false_, false_};
  REQUIRE(my_dfa->accepts(empty_trace));
  REQUIRE(my_dfa->accepts(trace_one));
  REQUIRE(my_dfa->accepts(trace_two));
}

TEST_CASE("Translate ff & tt)", "[translate]") {
  auto args = set_formulas({boolean(false), boolean(true)});
  auto ff_and_tt = std::make_shared<LDLfAnd>(args);
  auto formula_name = to_string(*ff_and_tt);
  // TODO complete.
  auto *my_dfa = to_dfa(*ff_and_tt);

  // print the DFA
  dfa_to_graphviz(*my_dfa, "translate_output_" + formula_name + ".svg", "svg");

  auto false_ = interpretation{};
  auto empty_trace = trace{};
  auto trace_one = trace{false_};
  auto trace_two = trace{false_, false_};
  REQUIRE(!my_dfa->accepts(empty_trace));
  REQUIRE(!my_dfa->accepts(trace_one));
  REQUIRE(!my_dfa->accepts(trace_two));
}

} // namespace whitemech::lydia::Test