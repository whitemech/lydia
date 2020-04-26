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

TEST_CASE("Translate (ff & tt)", "[translate]") {
  auto args = set_formulas({boolean(false), boolean(true)});
  auto ff_and_tt = std::make_shared<LDLfAnd>(args);
  auto formula_name = to_string(*ff_and_tt);

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

TEST_CASE("Translate <true>tt", "[translate]") {
  std::string formula_name = "<true>tt";
  auto true_ = std::make_shared<const PropositionalTrue>();
  auto regex_true = std::make_shared<const PropositionalRegExp>(true_);
  auto tt = boolean(true);
  auto diamond_formula_true_tt =
      std::make_shared<LDLfDiamond<PropositionalRegExp>>(regex_true, tt);

  auto *my_dfa = to_dfa(*diamond_formula_true_tt);

  // print the DFA
  dfa_to_graphviz(*my_dfa, "translate_output_" + formula_name + ".svg", "svg");

  auto false_ = interpretation{};
  REQUIRE(!my_dfa->accepts(trace{}));
  REQUIRE(my_dfa->accepts(trace{false_}));
  REQUIRE(my_dfa->accepts(trace{false_, false_}));
  REQUIRE(my_dfa->accepts(trace{false_, false_, false_}));
}

TEST_CASE("Translate <a>tt", "[translate]") {
  std::string formula_name = "<a>tt";
  auto a = std::make_shared<const PropositionalAtom>("a");
  auto regex_a = std::make_shared<const PropositionalRegExp>(a);
  auto tt = boolean(true);
  auto diamond_formula_a_tt =
      std::make_shared<LDLfDiamond<PropositionalRegExp>>(regex_a, tt);

  auto *my_dfa = to_dfa(*diamond_formula_a_tt);

  // print the DFA
  dfa_to_graphviz(*my_dfa, "translate_output_" + formula_name + ".svg", "svg");

  auto e = interpretation{0};
  auto a_ = interpretation{1};
  REQUIRE(!my_dfa->accepts(trace{}));
  REQUIRE(!my_dfa->accepts(trace{e}));
  REQUIRE(my_dfa->accepts(trace{a_}));
  REQUIRE(!my_dfa->accepts(trace{e, e}));
  REQUIRE(!my_dfa->accepts(trace{e, a_}));
  REQUIRE(my_dfa->accepts(trace{a_, e}));
  REQUIRE(my_dfa->accepts(trace{a_, a_}));
  REQUIRE(!my_dfa->accepts(trace{e, e, e}));
}

TEST_CASE("Translate <a & b>tt", "[translate]") {
  std::string formula_name = "<a & b>tt";
  auto a = std::make_shared<const PropositionalAtom>("a");
  auto b = std::make_shared<const PropositionalAtom>("b");
  auto a_and_b =
      std::make_shared<const PropositionalAnd>(set_prop_formulas{a, b});
  auto regex_a_and_b = std::make_shared<const PropositionalRegExp>(a_and_b);
  auto tt = boolean(true);
  auto diamond_formula_a_and_b_tt =
      std::make_shared<LDLfDiamond<PropositionalRegExp>>(regex_a_and_b, tt);

  auto *my_dfa = to_dfa(*diamond_formula_a_and_b_tt);

  // print the DFA
  dfa_to_graphviz(*my_dfa, "translate_output_" + formula_name + ".svg", "svg");

  auto e = interpretation{0, 0};
  auto a_ = interpretation{1, 0};
  auto b_ = interpretation{0, 1};
  auto ab_ = interpretation{1, 1};
  REQUIRE(!my_dfa->accepts(trace{}));
  REQUIRE(!my_dfa->accepts(trace{e}));
  REQUIRE(!my_dfa->accepts(trace{a_}));
  REQUIRE(!my_dfa->accepts(trace{b_}));
  REQUIRE(!my_dfa->accepts(trace{e, e}));
  REQUIRE(!my_dfa->accepts(trace{e, a_}));
  REQUIRE(!my_dfa->accepts(trace{a_, e}));
  REQUIRE(!my_dfa->accepts(trace{a_, a_}));
  REQUIRE(!my_dfa->accepts(trace{a_, a_}));
  REQUIRE(!my_dfa->accepts(trace{b_}));
  REQUIRE(my_dfa->accepts(trace{ab_}));
}

TEST_CASE("Translate {true}tt", "[translate]") {
  std::string formula_name = "[true]tt";
  auto true_ = std::make_shared<const PropositionalTrue>();
  auto regex_true = std::make_shared<const PropositionalRegExp>(true_);
  auto tt = boolean(true);
  auto box_formula_true_tt =
      std::make_shared<LDLfBox<PropositionalRegExp>>(regex_true, tt);

  auto *my_dfa = to_dfa(*box_formula_true_tt);

  // print the DFA
  dfa_to_graphviz(*my_dfa, "translate_output_" + formula_name + ".svg", "svg");

  auto false_ = interpretation{};
  REQUIRE(my_dfa->accepts(trace{}));
  REQUIRE(my_dfa->accepts(trace{false_}));
  REQUIRE(my_dfa->accepts(trace{false_, false_}));
  REQUIRE(my_dfa->accepts(trace{false_, false_, false_}));
}

TEST_CASE("Translate {a}tt", "[translate]") {
  std::string formula_name = "[a]tt";
  auto a = std::make_shared<const PropositionalAtom>("a");
  auto regex_a = std::make_shared<const PropositionalRegExp>(a);
  auto tt = boolean(true);
  auto box_formula_a_tt =
      std::make_shared<LDLfBox<PropositionalRegExp>>(regex_a, tt);

  auto *my_dfa = to_dfa(*box_formula_a_tt);

  // print the DFA
  dfa_to_graphviz(*my_dfa, "translate_output_" + formula_name + ".svg", "svg");

  auto e = interpretation{0};
  auto a_ = interpretation{1};
  REQUIRE(my_dfa->accepts(trace{}));
  REQUIRE(my_dfa->accepts(trace{e}));
  REQUIRE(my_dfa->accepts(trace{a_}));
  REQUIRE(my_dfa->accepts(trace{e, e}));
  REQUIRE(my_dfa->accepts(trace{e, a_}));
  REQUIRE(my_dfa->accepts(trace{a_, e}));
  REQUIRE(my_dfa->accepts(trace{a_, a_}));
  REQUIRE(my_dfa->accepts(trace{e, e, e}));
}

TEST_CASE("Translate {a & b}tt", "[translate]") {
  std::string formula_name = "[a & b]tt";
  auto a = std::make_shared<const PropositionalAtom>("a");
  auto b = std::make_shared<const PropositionalAtom>("b");
  auto a_and_b =
      std::make_shared<const PropositionalAnd>(set_prop_formulas{a, b});
  auto regex_a_and_b = std::make_shared<const PropositionalRegExp>(a_and_b);
  auto tt = boolean(true);
  auto box_formula_a_and_b_tt =
      std::make_shared<LDLfBox<PropositionalRegExp>>(regex_a_and_b, tt);

  auto *my_dfa = to_dfa(*box_formula_a_and_b_tt);

  // print the DFA
  dfa_to_graphviz(*my_dfa, "translate_output_" + formula_name + ".svg", "svg");

  auto e = interpretation{0, 0};
  auto a_ = interpretation{1, 0};
  auto b_ = interpretation{0, 1};
  auto ab_ = interpretation{1, 1};
  REQUIRE(my_dfa->accepts(trace{}));
  REQUIRE(my_dfa->accepts(trace{e}));
  REQUIRE(my_dfa->accepts(trace{a_}));
  REQUIRE(my_dfa->accepts(trace{b_}));
  REQUIRE(my_dfa->accepts(trace{e, e}));
  REQUIRE(my_dfa->accepts(trace{e, a_}));
  REQUIRE(my_dfa->accepts(trace{a_, e}));
  REQUIRE(my_dfa->accepts(trace{a_, a_}));
  REQUIRE(my_dfa->accepts(trace{a_, a_}));
  REQUIRE(my_dfa->accepts(trace{b_}));
  REQUIRE(my_dfa->accepts(trace{ab_}));
}

TEST_CASE("Translate {a}ff", "[translate]") {
  std::string formula_name = "[a]ff";
  auto a = std::make_shared<const PropositionalAtom>("a");
  auto regex_a = std::make_shared<const PropositionalRegExp>(a);
  auto tt = boolean(false);
  auto box_formula_a_tt =
      std::make_shared<LDLfBox<PropositionalRegExp>>(regex_a, tt);

  auto *my_dfa = to_dfa(*box_formula_a_tt);

  // print the DFA
  dfa_to_graphviz(*my_dfa, "translate_output_" + formula_name + ".svg", "svg");

  auto e = interpretation{0};
  auto a_ = interpretation{1};
  REQUIRE(my_dfa->accepts(trace{}));
  REQUIRE(my_dfa->accepts(trace{e}));
  REQUIRE(!my_dfa->accepts(trace{a_}));
  REQUIRE(my_dfa->accepts(trace{e, e}));
  REQUIRE(my_dfa->accepts(trace{e, a_}));
  REQUIRE(!my_dfa->accepts(trace{a_, e}));
  REQUIRE(!my_dfa->accepts(trace{a_, a_}));
  REQUIRE(my_dfa->accepts(trace{e, e, e}));
}

TEST_CASE("Translate <<true>tt?>tt", "[translate]") {
  std::string formula_name = "<<true>tt?>tt";
  auto true_ = std::make_shared<const PropositionalTrue>();
  auto regex_true = std::make_shared<const PropositionalRegExp>(true_);
  auto tt = boolean(true);
  auto diamond_formula_true_tt =
      std::make_shared<LDLfDiamond<PropositionalRegExp>>(regex_true, tt);
  auto regex_test = std::make_shared<const TestRegExp>(diamond_formula_true_tt);
  auto diamond_test = std::make_shared<LDLfDiamond<TestRegExp>>(regex_test, tt);

  auto *my_dfa = to_dfa(*diamond_test);

  // print the DFA
  dfa_to_graphviz(*my_dfa, "translate_output_" + formula_name + ".svg", "svg");

  auto false_ = interpretation{};
  REQUIRE(!my_dfa->accepts(trace{}));
  REQUIRE(my_dfa->accepts(trace{false_}));
  REQUIRE(my_dfa->accepts(trace{false_, false_}));
  REQUIRE(my_dfa->accepts(trace{false_, false_, false_}));
}

TEST_CASE("Translate <[true]ff?>tt", "[translate]") {
  std::string formula_name = "<[true]ff?>tt";
  auto true_ = std::make_shared<const PropositionalTrue>();
  auto regex_true = std::make_shared<const PropositionalRegExp>(true_);
  auto tt = boolean(true);
  auto ff = boolean(false);
  auto box_formula_true_ff =
      std::make_shared<LDLfBox<PropositionalRegExp>>(regex_true, ff);
  auto regex_test = std::make_shared<const TestRegExp>(box_formula_true_ff);
  auto diamond_test = std::make_shared<LDLfDiamond<TestRegExp>>(regex_test, tt);

  auto *my_dfa = to_dfa(*diamond_test);

  // print the DFA
  dfa_to_graphviz(*my_dfa, "translate_output_" + formula_name + ".svg", "svg");

  auto false_ = interpretation{};
  REQUIRE(my_dfa->accepts(trace{}));
  REQUIRE(!my_dfa->accepts(trace{false_}));
  REQUIRE(!my_dfa->accepts(trace{false_, false_}));
  REQUIRE(!my_dfa->accepts(trace{false_, false_, false_}));
}

} // namespace whitemech::lydia::Test