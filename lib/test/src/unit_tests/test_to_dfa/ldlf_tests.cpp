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

#include "test/src/utils/to_dfa.hpp"
#include <iostream>
#include <lydia/logic/nnf.hpp>
#include <lydia/to_dfa/dfa_state.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("Set of DFA states", "[translate][symbolic]") {
  auto context = AstManager{};
  auto a = DFAState(context, set_nfa_states{});
  auto b = DFAState(context, set_formulas{context.makeLdlfTrue()});
  auto c = DFAState(context, set_nfa_states{});

  REQUIRE(a == c);
  REQUIRE(c == a);
  REQUIRE(!(a == b));
  REQUIRE(a < b);
}

TEST_CASE("Translate !(ff & tt)", "[translate][ldlf][basic]") {
  std::string formula_name = "!(ff & tt)";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, true));
  REQUIRE(verify(*automaton, {{}}, true));
  REQUIRE(verify(*automaton, {{}, {}}, true));
}

TEST_CASE("Translate (ff & tt)", "[translate][ldlf][basic]") {
  std::string formula_name = "(ff & tt)";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, false));
  REQUIRE(verify(*automaton, {{}}, false));
  REQUIRE(verify(*automaton, {{}, {}}, false));
}

TEST_CASE("Translate <true>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<true>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {{}}, true));
  REQUIRE(verify(*automaton, {{}, {}}, true));
}

TEST_CASE("Translate <a>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<a>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"0"}, false));
  REQUIRE(verify(*automaton, {"1"}, true));
  REQUIRE(verify(*automaton, {"0", "0"}, false));
  REQUIRE(verify(*automaton, {"0", "1"}, false));
  REQUIRE(verify(*automaton, {"1", "0"}, true));
  REQUIRE(verify(*automaton, {"1", "1"}, true));
}

TEST_CASE("Translate <a & b>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<a & b>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"00"}, false));
  REQUIRE(verify(*automaton, {"01"}, false));
  REQUIRE(verify(*automaton, {"10"}, false));
  REQUIRE(verify(*automaton, {"11"}, true));

  REQUIRE(verify(*automaton, {"00", "00"}, false));
  REQUIRE(verify(*automaton, {"00", "01"}, false));
  REQUIRE(verify(*automaton, {"00", "10"}, false));
  REQUIRE(verify(*automaton, {"00", "11"}, false));

  REQUIRE(verify(*automaton, {"01", "00"}, false));
  REQUIRE(verify(*automaton, {"01", "01"}, false));
  REQUIRE(verify(*automaton, {"01", "10"}, false));
  REQUIRE(verify(*automaton, {"01", "11"}, false));

  REQUIRE(verify(*automaton, {"10", "00"}, false));
  REQUIRE(verify(*automaton, {"10", "01"}, false));
  REQUIRE(verify(*automaton, {"10", "10"}, false));
  REQUIRE(verify(*automaton, {"10", "11"}, false));

  REQUIRE(verify(*automaton, {"11", "00"}, true));
  REQUIRE(verify(*automaton, {"11", "01"}, true));
  REQUIRE(verify(*automaton, {"11", "10"}, true));
  REQUIRE(verify(*automaton, {"11", "11"}, true));
}

TEST_CASE("Translate <a | b>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<a | b>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"00"}, false));
  REQUIRE(verify(*automaton, {"01"}, true));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, true));

  REQUIRE(verify(*automaton, {"00", "00"}, false));
  REQUIRE(verify(*automaton, {"00", "01"}, false));
  REQUIRE(verify(*automaton, {"00", "10"}, false));
  REQUIRE(verify(*automaton, {"00", "11"}, false));

  REQUIRE(verify(*automaton, {"01", "00"}, true));
  REQUIRE(verify(*automaton, {"01", "01"}, true));
  REQUIRE(verify(*automaton, {"01", "10"}, true));
  REQUIRE(verify(*automaton, {"01", "11"}, true));

  REQUIRE(verify(*automaton, {"10", "00"}, true));
  REQUIRE(verify(*automaton, {"10", "01"}, true));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));

  REQUIRE(verify(*automaton, {"11", "00"}, true));
  REQUIRE(verify(*automaton, {"11", "01"}, true));
  REQUIRE(verify(*automaton, {"11", "10"}, true));
  REQUIRE(verify(*automaton, {"11", "11"}, true));
}

TEST_CASE("Translate <!a>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<!a>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"0"}, true));
  REQUIRE(verify(*automaton, {"1"}, false));
  REQUIRE(verify(*automaton, {"0", "0"}, true));
  REQUIRE(verify(*automaton, {"0", "1"}, true));
  REQUIRE(verify(*automaton, {"1", "0"}, false));
  REQUIRE(verify(*automaton, {"1", "1"}, false));
}

TEST_CASE("Translate <!(a & b)>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<!(a & b)>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"00"}, true));
  REQUIRE(verify(*automaton, {"01"}, true));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, false));

  REQUIRE(verify(*automaton, {"00", "00"}, true));
  REQUIRE(verify(*automaton, {"00", "01"}, true));
  REQUIRE(verify(*automaton, {"00", "10"}, true));
  REQUIRE(verify(*automaton, {"00", "11"}, true));

  REQUIRE(verify(*automaton, {"01", "00"}, true));
  REQUIRE(verify(*automaton, {"01", "01"}, true));
  REQUIRE(verify(*automaton, {"01", "10"}, true));
  REQUIRE(verify(*automaton, {"01", "11"}, true));

  REQUIRE(verify(*automaton, {"10", "00"}, true));
  REQUIRE(verify(*automaton, {"10", "01"}, true));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));

  REQUIRE(verify(*automaton, {"11", "00"}, false));
  REQUIRE(verify(*automaton, {"11", "01"}, false));
  REQUIRE(verify(*automaton, {"11", "10"}, false));
  REQUIRE(verify(*automaton, {"11", "11"}, false));
}

TEST_CASE("Translate <!(a | b)>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<!(a | b)>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"00"}, true));
  REQUIRE(verify(*automaton, {"01"}, false));
  REQUIRE(verify(*automaton, {"10"}, false));
  REQUIRE(verify(*automaton, {"11"}, false));

  REQUIRE(verify(*automaton, {"00", "00"}, true));
  REQUIRE(verify(*automaton, {"00", "01"}, true));
  REQUIRE(verify(*automaton, {"00", "10"}, true));
  REQUIRE(verify(*automaton, {"00", "11"}, true));

  REQUIRE(verify(*automaton, {"01", "00"}, false));
  REQUIRE(verify(*automaton, {"01", "01"}, false));
  REQUIRE(verify(*automaton, {"01", "10"}, false));
  REQUIRE(verify(*automaton, {"01", "11"}, false));

  REQUIRE(verify(*automaton, {"10", "00"}, false));
  REQUIRE(verify(*automaton, {"10", "01"}, false));
  REQUIRE(verify(*automaton, {"10", "10"}, false));
  REQUIRE(verify(*automaton, {"10", "11"}, false));

  REQUIRE(verify(*automaton, {"11", "00"}, false));
  REQUIRE(verify(*automaton, {"11", "01"}, false));
  REQUIRE(verify(*automaton, {"11", "10"}, false));
  REQUIRE(verify(*automaton, {"11", "11"}, false));
}

TEST_CASE("Translate {true}tt", "[translate][ldlf][basic]") {
  std::string formula_name = "[true]tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, true));
  REQUIRE(verify(*automaton, {{}}, true));
  REQUIRE(verify(*automaton, {{}, {}}, true));
}

TEST_CASE("Translate {a}tt", "[translate][ldlf][basic]") {
  std::string formula_name = "[a]tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, true));
  REQUIRE(verify(*automaton, {"0"}, true));
  REQUIRE(verify(*automaton, {"1"}, true));
  REQUIRE(verify(*automaton, {"0", "0"}, true));
  REQUIRE(verify(*automaton, {"0", "1"}, true));
  REQUIRE(verify(*automaton, {"1", "0"}, true));
  REQUIRE(verify(*automaton, {"1", "1"}, true));
}

TEST_CASE("Translate {a & b}ff", "[translate][ldlf][basic]") {
  std::string formula_name = "[a & b]ff";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, true));

  REQUIRE(verify(*automaton, {"00"}, true));
  REQUIRE(verify(*automaton, {"01"}, true));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, false));

  REQUIRE(verify(*automaton, {"00", "00"}, true));
  REQUIRE(verify(*automaton, {"00", "01"}, true));
  REQUIRE(verify(*automaton, {"00", "10"}, true));
  REQUIRE(verify(*automaton, {"00", "11"}, true));

  REQUIRE(verify(*automaton, {"01", "00"}, true));
  REQUIRE(verify(*automaton, {"01", "01"}, true));
  REQUIRE(verify(*automaton, {"01", "10"}, true));
  REQUIRE(verify(*automaton, {"01", "11"}, true));

  REQUIRE(verify(*automaton, {"10", "00"}, true));
  REQUIRE(verify(*automaton, {"10", "01"}, true));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));

  REQUIRE(verify(*automaton, {"11", "00"}, false));
  REQUIRE(verify(*automaton, {"11", "01"}, false));
  REQUIRE(verify(*automaton, {"11", "10"}, false));
  REQUIRE(verify(*automaton, {"11", "11"}, false));
}

TEST_CASE("Translate {a | b}ff", "[translate][ldlf][basic]") {
  std::string formula_name = "[a | b]ff";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, true));

  REQUIRE(verify(*automaton, {"00"}, true));
  REQUIRE(verify(*automaton, {"01"}, false));
  REQUIRE(verify(*automaton, {"10"}, false));
  REQUIRE(verify(*automaton, {"11"}, false));

  REQUIRE(verify(*automaton, {"00", "00"}, true));
  REQUIRE(verify(*automaton, {"00", "01"}, true));
  REQUIRE(verify(*automaton, {"00", "10"}, true));
  REQUIRE(verify(*automaton, {"00", "11"}, true));

  REQUIRE(verify(*automaton, {"01", "00"}, false));
  REQUIRE(verify(*automaton, {"01", "01"}, false));
  REQUIRE(verify(*automaton, {"01", "10"}, false));
  REQUIRE(verify(*automaton, {"01", "11"}, false));

  REQUIRE(verify(*automaton, {"10", "00"}, false));
  REQUIRE(verify(*automaton, {"10", "01"}, false));
  REQUIRE(verify(*automaton, {"10", "10"}, false));
  REQUIRE(verify(*automaton, {"10", "11"}, false));

  REQUIRE(verify(*automaton, {"11", "00"}, false));
  REQUIRE(verify(*automaton, {"11", "01"}, false));
  REQUIRE(verify(*automaton, {"11", "10"}, false));
  REQUIRE(verify(*automaton, {"11", "11"}, false));
}

TEST_CASE("Translate {a}ff", "[translate][ldlf][basic]") {
  std::string formula_name = "[a]ff";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, true));
  REQUIRE(verify(*automaton, {"0"}, true));
  REQUIRE(verify(*automaton, {"1"}, false));
  REQUIRE(verify(*automaton, {"0", "0"}, true));
  REQUIRE(verify(*automaton, {"0", "1"}, true));
  REQUIRE(verify(*automaton, {"1", "0"}, false));
  REQUIRE(verify(*automaton, {"1", "1"}, false));
}

TEST_CASE("Translate <<true>tt?>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<<true>tt?>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));
  REQUIRE(verify(*automaton, {{}}, true));
  REQUIRE(verify(*automaton, {{}, {}}, true));
}

TEST_CASE("Translate <{true}ff?>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<[true]ff?>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, true));
  REQUIRE(verify(*automaton, {{}}, false));
  REQUIRE(verify(*automaton, {{}, {}}, false));
}

TEST_CASE("Translate <a plus b>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<a + b>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"00"}, false));
  REQUIRE(verify(*automaton, {"01"}, true));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, true));

  REQUIRE(verify(*automaton, {"00", "00"}, false));
  REQUIRE(verify(*automaton, {"00", "01"}, false));
  REQUIRE(verify(*automaton, {"00", "10"}, false));
  REQUIRE(verify(*automaton, {"00", "11"}, false));

  REQUIRE(verify(*automaton, {"01", "00"}, true));
  REQUIRE(verify(*automaton, {"01", "01"}, true));
  REQUIRE(verify(*automaton, {"01", "10"}, true));
  REQUIRE(verify(*automaton, {"01", "11"}, true));

  REQUIRE(verify(*automaton, {"10", "00"}, true));
  REQUIRE(verify(*automaton, {"10", "01"}, true));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));

  REQUIRE(verify(*automaton, {"11", "00"}, true));
  REQUIRE(verify(*automaton, {"11", "01"}, true));
  REQUIRE(verify(*automaton, {"11", "10"}, true));
  REQUIRE(verify(*automaton, {"11", "11"}, true));
}

TEST_CASE("Translate {a plus b}ff", "[translate][ldlf][basic]") {
  std::string formula_name = "[a + b]ff";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, true));

  REQUIRE(verify(*automaton, {"00"}, true));
  REQUIRE(verify(*automaton, {"01"}, false));
  REQUIRE(verify(*automaton, {"10"}, false));
  REQUIRE(verify(*automaton, {"11"}, false));

  REQUIRE(verify(*automaton, {"00", "00"}, true));
  REQUIRE(verify(*automaton, {"00", "01"}, true));
  REQUIRE(verify(*automaton, {"00", "10"}, true));
  REQUIRE(verify(*automaton, {"00", "11"}, true));

  REQUIRE(verify(*automaton, {"01", "00"}, false));
  REQUIRE(verify(*automaton, {"01", "01"}, false));
  REQUIRE(verify(*automaton, {"01", "10"}, false));
  REQUIRE(verify(*automaton, {"01", "11"}, false));

  REQUIRE(verify(*automaton, {"10", "00"}, false));
  REQUIRE(verify(*automaton, {"10", "01"}, false));
  REQUIRE(verify(*automaton, {"10", "10"}, false));
  REQUIRE(verify(*automaton, {"10", "11"}, false));

  REQUIRE(verify(*automaton, {"11", "00"}, false));
  REQUIRE(verify(*automaton, {"11", "01"}, false));
  REQUIRE(verify(*automaton, {"11", "10"}, false));
  REQUIRE(verify(*automaton, {"11", "11"}, false));
}

TEST_CASE("Translate <a,b>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<a ; b>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"00"}, false));
  REQUIRE(verify(*automaton, {"01"}, false));
  REQUIRE(verify(*automaton, {"10"}, false));
  REQUIRE(verify(*automaton, {"11"}, false));

  REQUIRE(verify(*automaton, {"00", "00"}, false));
  REQUIRE(verify(*automaton, {"00", "01"}, false));
  REQUIRE(verify(*automaton, {"00", "10"}, false));
  REQUIRE(verify(*automaton, {"00", "11"}, false));

  REQUIRE(verify(*automaton, {"01", "00"}, false));
  REQUIRE(verify(*automaton, {"01", "01"}, false));
  REQUIRE(verify(*automaton, {"01", "10"}, true));
  REQUIRE(verify(*automaton, {"01", "11"}, true));

  REQUIRE(verify(*automaton, {"10", "00"}, false));
  REQUIRE(verify(*automaton, {"10", "01"}, false));
  REQUIRE(verify(*automaton, {"10", "10"}, false));
  REQUIRE(verify(*automaton, {"10", "11"}, false));

  REQUIRE(verify(*automaton, {"11", "00"}, false));
  REQUIRE(verify(*automaton, {"11", "01"}, false));
  REQUIRE(verify(*automaton, {"11", "10"}, true));
  REQUIRE(verify(*automaton, {"11", "11"}, true));
}

TEST_CASE("Translate {a,b}ff", "[translate][ldlf][basic]") {
  std::string formula_name = "[a ; b]ff";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, true));

  REQUIRE(verify(*automaton, {"00"}, true));
  REQUIRE(verify(*automaton, {"01"}, true));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, true));

  REQUIRE(verify(*automaton, {"00", "00"}, true));
  REQUIRE(verify(*automaton, {"00", "01"}, true));
  REQUIRE(verify(*automaton, {"00", "10"}, true));
  REQUIRE(verify(*automaton, {"00", "11"}, true));

  REQUIRE(verify(*automaton, {"01", "00"}, true));
  REQUIRE(verify(*automaton, {"01", "01"}, true));
  REQUIRE(verify(*automaton, {"01", "10"}, false));
  REQUIRE(verify(*automaton, {"01", "11"}, false));

  REQUIRE(verify(*automaton, {"10", "00"}, true));
  REQUIRE(verify(*automaton, {"10", "01"}, true));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));

  REQUIRE(verify(*automaton, {"11", "00"}, true));
  REQUIRE(verify(*automaton, {"11", "01"}, true));
  REQUIRE(verify(*automaton, {"11", "10"}, false));
  REQUIRE(verify(*automaton, {"11", "11"}, false));
}

TEST_CASE("Translate <a*>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<a*>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, true));

  REQUIRE(verify(*automaton, {"00"}, true));
  REQUIRE(verify(*automaton, {"01"}, true));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, true));

  REQUIRE(verify(*automaton, {"00", "00"}, true));
  REQUIRE(verify(*automaton, {"00", "01"}, true));
  REQUIRE(verify(*automaton, {"00", "10"}, true));
  REQUIRE(verify(*automaton, {"00", "11"}, true));

  REQUIRE(verify(*automaton, {"01", "00"}, true));
  REQUIRE(verify(*automaton, {"01", "01"}, true));
  REQUIRE(verify(*automaton, {"01", "10"}, true));
  REQUIRE(verify(*automaton, {"01", "11"}, true));

  REQUIRE(verify(*automaton, {"10", "00"}, true));
  REQUIRE(verify(*automaton, {"10", "01"}, true));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));

  REQUIRE(verify(*automaton, {"11", "00"}, true));
  REQUIRE(verify(*automaton, {"11", "01"}, true));
  REQUIRE(verify(*automaton, {"11", "10"}, true));
  REQUIRE(verify(*automaton, {"11", "11"}, true));
}

TEST_CASE("Translate {a*}tt", "[translate][ldlf][basic]") {
  std::string formula_name = "[a*]tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, true));

  REQUIRE(verify(*automaton, {"00"}, true));
  REQUIRE(verify(*automaton, {"01"}, true));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, true));

  REQUIRE(verify(*automaton, {"00", "00"}, true));
  REQUIRE(verify(*automaton, {"00", "01"}, true));
  REQUIRE(verify(*automaton, {"00", "10"}, true));
  REQUIRE(verify(*automaton, {"00", "11"}, true));

  REQUIRE(verify(*automaton, {"01", "00"}, true));
  REQUIRE(verify(*automaton, {"01", "01"}, true));
  REQUIRE(verify(*automaton, {"01", "10"}, true));
  REQUIRE(verify(*automaton, {"01", "11"}, true));

  REQUIRE(verify(*automaton, {"10", "00"}, true));
  REQUIRE(verify(*automaton, {"10", "01"}, true));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));

  REQUIRE(verify(*automaton, {"11", "00"}, true));
  REQUIRE(verify(*automaton, {"11", "01"}, true));
  REQUIRE(verify(*automaton, {"11", "10"}, true));
  REQUIRE(verify(*automaton, {"11", "11"}, true));
}

TEST_CASE("Translate <a*, b>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<a*; b>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"00"}, false));
  REQUIRE(verify(*automaton, {"01"}, false));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, true));

  REQUIRE(verify(*automaton, {"00", "00"}, false));
  REQUIRE(verify(*automaton, {"00", "01"}, false));
  REQUIRE(verify(*automaton, {"00", "10"}, false));
  REQUIRE(verify(*automaton, {"00", "11"}, false));

  REQUIRE(verify(*automaton, {"01", "00"}, false));
  REQUIRE(verify(*automaton, {"01", "01"}, false));
  REQUIRE(verify(*automaton, {"01", "10"}, true));
  REQUIRE(verify(*automaton, {"01", "11"}, true));

  REQUIRE(verify(*automaton, {"10", "00"}, true));
  REQUIRE(verify(*automaton, {"10", "01"}, true));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));

  REQUIRE(verify(*automaton, {"11", "00"}, true));
  REQUIRE(verify(*automaton, {"11", "01"}, true));
  REQUIRE(verify(*automaton, {"11", "10"}, true));
  REQUIRE(verify(*automaton, {"11", "11"}, true));
}

TEST_CASE("Translate {a*, b}ff", "[translate][ldlf][basic]") {
  std::string formula_name = "[a*; b]ff";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, true));

  REQUIRE(verify(*automaton, {"00"}, true));
  REQUIRE(verify(*automaton, {"01"}, true));
  REQUIRE(verify(*automaton, {"10"}, false));
  REQUIRE(verify(*automaton, {"11"}, false));

  REQUIRE(verify(*automaton, {"00", "00"}, true));
  REQUIRE(verify(*automaton, {"00", "01"}, true));
  REQUIRE(verify(*automaton, {"00", "10"}, true));
  REQUIRE(verify(*automaton, {"00", "11"}, true));

  REQUIRE(verify(*automaton, {"01", "00"}, true));
  REQUIRE(verify(*automaton, {"01", "01"}, true));
  REQUIRE(verify(*automaton, {"01", "10"}, false));
  REQUIRE(verify(*automaton, {"01", "11"}, false));

  REQUIRE(verify(*automaton, {"10", "00"}, false));
  REQUIRE(verify(*automaton, {"10", "01"}, false));
  REQUIRE(verify(*automaton, {"10", "10"}, false));
  REQUIRE(verify(*automaton, {"10", "11"}, false));

  REQUIRE(verify(*automaton, {"11", "00"}, false));
  REQUIRE(verify(*automaton, {"11", "01"}, false));
  REQUIRE(verify(*automaton, {"11", "10"}, false));
  REQUIRE(verify(*automaton, {"11", "11"}, false));
}

TEST_CASE("Translate <a* plus b>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<a* + b>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, true));

  REQUIRE(verify(*automaton, {"00"}, true));
  REQUIRE(verify(*automaton, {"01"}, true));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, true));

  REQUIRE(verify(*automaton, {"00", "00"}, true));
  REQUIRE(verify(*automaton, {"00", "01"}, true));
  REQUIRE(verify(*automaton, {"00", "10"}, true));
  REQUIRE(verify(*automaton, {"00", "11"}, true));

  REQUIRE(verify(*automaton, {"01", "00"}, true));
  REQUIRE(verify(*automaton, {"01", "01"}, true));
  REQUIRE(verify(*automaton, {"01", "10"}, true));
  REQUIRE(verify(*automaton, {"01", "11"}, true));

  REQUIRE(verify(*automaton, {"10", "00"}, true));
  REQUIRE(verify(*automaton, {"10", "01"}, true));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));

  REQUIRE(verify(*automaton, {"11", "00"}, true));
  REQUIRE(verify(*automaton, {"11", "01"}, true));
  REQUIRE(verify(*automaton, {"11", "10"}, true));
  REQUIRE(verify(*automaton, {"11", "11"}, true));
}

TEST_CASE("Translate {a* plus b}ff", "[translate][ldlf][basic]") {
  std::string formula_name = "[a* + b]ff";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"00"}, false));
  REQUIRE(verify(*automaton, {"01"}, false));
  REQUIRE(verify(*automaton, {"10"}, false));
  REQUIRE(verify(*automaton, {"11"}, false));

  REQUIRE(verify(*automaton, {"00", "00"}, false));
  REQUIRE(verify(*automaton, {"00", "01"}, false));
  REQUIRE(verify(*automaton, {"00", "10"}, false));
  REQUIRE(verify(*automaton, {"00", "11"}, false));

  REQUIRE(verify(*automaton, {"01", "00"}, false));
  REQUIRE(verify(*automaton, {"01", "01"}, false));
  REQUIRE(verify(*automaton, {"01", "10"}, false));
  REQUIRE(verify(*automaton, {"01", "11"}, false));

  REQUIRE(verify(*automaton, {"10", "00"}, false));
  REQUIRE(verify(*automaton, {"10", "01"}, false));
  REQUIRE(verify(*automaton, {"10", "10"}, false));
  REQUIRE(verify(*automaton, {"10", "11"}, false));

  REQUIRE(verify(*automaton, {"11", "00"}, false));
  REQUIRE(verify(*automaton, {"11", "01"}, false));
  REQUIRE(verify(*automaton, {"11", "10"}, false));
  REQUIRE(verify(*automaton, {"11", "11"}, false));
}

TEST_CASE("Translate <true*>(<a>tt & !end)", "[translate][ldlf][basic]") {
  std::string formula_name = "<true*>(<a>tt & !end)";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"0"}, false));
  REQUIRE(verify(*automaton, {"1"}, true));

  REQUIRE(verify(*automaton, {"0", "0"}, false));
  REQUIRE(verify(*automaton, {"0", "1"}, true));
  REQUIRE(verify(*automaton, {"1", "0"}, true));
  REQUIRE(verify(*automaton, {"1", "1"}, true));

  REQUIRE(verify(*automaton, {"0", "0", "0"}, false));
  REQUIRE(verify(*automaton, {"0", "0", "1"}, true));
  REQUIRE(verify(*automaton, {"0", "1", "0"}, true));
  REQUIRE(verify(*automaton, {"0", "1", "1"}, true));
  REQUIRE(verify(*automaton, {"1", "0", "0"}, true));
  REQUIRE(verify(*automaton, {"1", "0", "1"}, true));
  REQUIRE(verify(*automaton, {"1", "1", "0"}, true));
  REQUIRE(verify(*automaton, {"1", "1", "1"}, true));
}

TEST_CASE("Translate <(<a>tt?, true)*>(<b>tt & !end)",
          "[translate][ldlf][basic]") {
  std::string formula_name = "<(<a>tt?; true)*>(<b>tt & !end)";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"00"}, false));
  REQUIRE(verify(*automaton, {"01"}, false));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, true));

  REQUIRE(verify(*automaton, {"00", "00"}, false));
  REQUIRE(verify(*automaton, {"00", "01"}, false));
  REQUIRE(verify(*automaton, {"00", "10"}, false));
  REQUIRE(verify(*automaton, {"00", "11"}, false));

  REQUIRE(verify(*automaton, {"01", "00"}, false));
  REQUIRE(verify(*automaton, {"01", "01"}, false));
  REQUIRE(verify(*automaton, {"01", "10"}, true));
  REQUIRE(verify(*automaton, {"01", "11"}, true));

  REQUIRE(verify(*automaton, {"10", "00"}, true));
  REQUIRE(verify(*automaton, {"10", "01"}, true));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));

  REQUIRE(verify(*automaton, {"11", "00"}, true));
  REQUIRE(verify(*automaton, {"11", "01"}, true));
  REQUIRE(verify(*automaton, {"11", "10"}, true));
  REQUIRE(verify(*automaton, {"11", "11"}, true));

  // corner cases
  REQUIRE(verify(*automaton, {"01", "01", "10"}, true));
  REQUIRE(verify(*automaton, {"01", "00", "10"}, false));
}

TEST_CASE("Translate <(<a>tt?,<c>tt?, true)*>(<b>tt & !end)",
          "[translate][ldlf][basic]") {
  std::string formula_name = "<(<a>tt?;<c>tt?; true)*>(<b>tt & !end)";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"000"}, false));
  REQUIRE(verify(*automaton, {"001"}, false));
  REQUIRE(verify(*automaton, {"010"}, true));
  REQUIRE(verify(*automaton, {"011"}, true));
  REQUIRE(verify(*automaton, {"100"}, false));
  REQUIRE(verify(*automaton, {"101"}, false));
  REQUIRE(verify(*automaton, {"110"}, true));
  REQUIRE(verify(*automaton, {"111"}, true));

  REQUIRE(verify(*automaton, {"000", "000"}, false));
  REQUIRE(verify(*automaton, {"000", "001"}, false));
  REQUIRE(verify(*automaton, {"000", "010"}, false));
  REQUIRE(verify(*automaton, {"000", "011"}, false));
  REQUIRE(verify(*automaton, {"000", "100"}, false));
  REQUIRE(verify(*automaton, {"000", "101"}, false));
  REQUIRE(verify(*automaton, {"000", "110"}, false));
  REQUIRE(verify(*automaton, {"000", "111"}, false));

  REQUIRE(verify(*automaton, {"001", "000"}, false));
  REQUIRE(verify(*automaton, {"001", "001"}, false));
  REQUIRE(verify(*automaton, {"001", "010"}, false));
  REQUIRE(verify(*automaton, {"001", "011"}, false));
  REQUIRE(verify(*automaton, {"001", "100"}, false));
  REQUIRE(verify(*automaton, {"001", "101"}, false));
  REQUIRE(verify(*automaton, {"001", "110"}, false));
  REQUIRE(verify(*automaton, {"001", "111"}, false));

  REQUIRE(verify(*automaton, {"100", "000"}, false));
  REQUIRE(verify(*automaton, {"100", "001"}, false));
  REQUIRE(verify(*automaton, {"100", "010"}, false));
  REQUIRE(verify(*automaton, {"100", "011"}, false));
  REQUIRE(verify(*automaton, {"100", "100"}, false));
  REQUIRE(verify(*automaton, {"100", "101"}, false));
  REQUIRE(verify(*automaton, {"100", "110"}, false));
  REQUIRE(verify(*automaton, {"100", "111"}, false));

  REQUIRE(verify(*automaton, {"101", "000"}, false));
  REQUIRE(verify(*automaton, {"101", "001"}, false));
  REQUIRE(verify(*automaton, {"101", "010"}, true));
  REQUIRE(verify(*automaton, {"101", "011"}, true));
  REQUIRE(verify(*automaton, {"101", "100"}, false));
  REQUIRE(verify(*automaton, {"101", "101"}, false));
  REQUIRE(verify(*automaton, {"101", "110"}, true));
  REQUIRE(verify(*automaton, {"101", "111"}, true));

  REQUIRE(verify(*automaton, {"010", "000"}, true));
  REQUIRE(verify(*automaton, {"010", "001"}, true));
  REQUIRE(verify(*automaton, {"010", "010"}, true));
  REQUIRE(verify(*automaton, {"010", "011"}, true));
  REQUIRE(verify(*automaton, {"010", "100"}, true));
  REQUIRE(verify(*automaton, {"010", "101"}, true));
  REQUIRE(verify(*automaton, {"010", "110"}, true));
  REQUIRE(verify(*automaton, {"010", "111"}, true));

  // corner cases
  REQUIRE(verify(*automaton, {"001", "001", "010"}, false));
  REQUIRE(verify(*automaton, {"001", "000", "010"}, false));
  REQUIRE(verify(*automaton, {"100", "100", "010"}, false));
  REQUIRE(verify(*automaton, {"100", "000", "010"}, false));
  REQUIRE(verify(*automaton, {"101", "101", "010"}, true));
  REQUIRE(verify(*automaton, {"101", "000", "010"}, false));
}

TEST_CASE("Translate with star test a,a and body b",
          "[translate][ldlf][basic]") {
  std::string formula_name = "<(<a;a>tt?; true)*>(<b>tt & !end)";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"00"}, false));
  REQUIRE(verify(*automaton, {"01"}, false));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, true));

  REQUIRE(verify(*automaton, {"00", "00"}, false));
  REQUIRE(verify(*automaton, {"00", "01"}, false));
  REQUIRE(verify(*automaton, {"00", "10"}, false));
  REQUIRE(verify(*automaton, {"00", "11"}, false));

  REQUIRE(verify(*automaton, {"01", "00"}, false));
  REQUIRE(verify(*automaton, {"01", "01"}, false));
  REQUIRE(verify(*automaton, {"01", "10"}, false));
  REQUIRE(verify(*automaton, {"01", "11"}, true));

  REQUIRE(verify(*automaton, {"10", "00"}, true));
  REQUIRE(verify(*automaton, {"10", "01"}, true));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));

  REQUIRE(verify(*automaton, {"11", "00"}, true));
  REQUIRE(verify(*automaton, {"11", "01"}, true));
  REQUIRE(verify(*automaton, {"11", "10"}, true));
  REQUIRE(verify(*automaton, {"11", "11"}, true));

  // corner cases
  REQUIRE(verify(*automaton, {"01", "01", "10"}, false));
  REQUIRE(verify(*automaton, {"01", "01", "11", "01"}, true));
  REQUIRE(verify(*automaton, {"01", "00", "10"}, false));
}

TEST_CASE("Translate {true*}(<a>tt | end)", "[translate][ldlf][basic]") {
  std::string formula_name = "[true*](<a>tt | end)";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, true));

  REQUIRE(verify(*automaton, {"0"}, false));
  REQUIRE(verify(*automaton, {"1"}, true));

  REQUIRE(verify(*automaton, {"0", "0"}, false));
  REQUIRE(verify(*automaton, {"0", "1"}, false));
  REQUIRE(verify(*automaton, {"1", "0"}, false));
  REQUIRE(verify(*automaton, {"1", "1"}, true));

  REQUIRE(verify(*automaton, {"0", "0", "0"}, false));
  REQUIRE(verify(*automaton, {"0", "0", "1"}, false));
  REQUIRE(verify(*automaton, {"0", "1", "0"}, false));
  REQUIRE(verify(*automaton, {"0", "1", "1"}, false));
  REQUIRE(verify(*automaton, {"1", "0", "0"}, false));
  REQUIRE(verify(*automaton, {"1", "0", "1"}, false));
  REQUIRE(verify(*automaton, {"1", "1", "0"}, false));
  REQUIRE(verify(*automaton, {"1", "1", "1"}, true));
}

TEST_CASE("Translate <(<a>tt?)*><b>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<(<a>tt?)*><b>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"00"}, false));
  REQUIRE(verify(*automaton, {"01"}, false));
  REQUIRE(verify(*automaton, {"10"}, true));
  REQUIRE(verify(*automaton, {"11"}, true));

  REQUIRE(verify(*automaton, {"00", "00"}, false));
  REQUIRE(verify(*automaton, {"00", "01"}, false));
  REQUIRE(verify(*automaton, {"00", "10"}, false));
  REQUIRE(verify(*automaton, {"00", "11"}, false));

  REQUIRE(verify(*automaton, {"01", "00"}, false));
  REQUIRE(verify(*automaton, {"01", "01"}, false));
  REQUIRE(verify(*automaton, {"01", "10"}, false));
  REQUIRE(verify(*automaton, {"01", "11"}, false));

  REQUIRE(verify(*automaton, {"10", "00"}, true));
  REQUIRE(verify(*automaton, {"10", "01"}, true));
  REQUIRE(verify(*automaton, {"10", "10"}, true));
  REQUIRE(verify(*automaton, {"10", "11"}, true));

  REQUIRE(verify(*automaton, {"11", "00"}, true));
  REQUIRE(verify(*automaton, {"11", "01"}, true));
  REQUIRE(verify(*automaton, {"11", "10"}, true));
  REQUIRE(verify(*automaton, {"11", "11"}, true));
}

TEST_CASE("Translate <a,b + c,d>tt", "[translate][ldlf][basic]") {
  std::string formula_name = "<(a;b) + (c;d)>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
}

TEST_CASE("Translate sequence of stars", "[translate][ldlf][basic]") {
  std::string formula_name = "<p_10* ; p_11* ; p_12* ; p_13* ; p_14*>tt";
  auto strategy_maker = GENERATE(strategies());
  auto mgr = CUDD::Cudd();
  auto strategy = strategy_maker(mgr);
  auto automaton = to_dfa_from_formula_string(formula_name, *strategy);
  //  print_dfa(*automaton, formula_name);
}

} // namespace whitemech::lydia::Test