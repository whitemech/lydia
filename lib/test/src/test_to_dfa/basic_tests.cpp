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
#include "../utils/to_dfa.hpp"
#include <catch.hpp>
#include <iostream>
#include <lydia/nnf.hpp>
#include <lydia/to_dfa/dfa_state.hpp>

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
  std::string formula_name = "!(ff & tt)";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, true));
  REQUIRE(verify(*automaton, {{}}, true));
  REQUIRE(verify(*automaton, {{}, {}}, true));
}

TEST_CASE("Translate (ff & tt)", "[translate]") {
  std::string formula_name = "(ff & tt)";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, false));
  REQUIRE(verify(*automaton, {{}}, false));
  REQUIRE(verify(*automaton, {{}, {}}, false));
}

TEST_CASE("Translate <true>tt", "[translate]") {
  std::string formula_name = "<true>tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, false));
  REQUIRE(verify(*automaton, {{}}, true));
  REQUIRE(verify(*automaton, {{}, {}}, true));
}

TEST_CASE("Translate <a>tt", "[translate]") {
  std::string formula_name = "<a>tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));

  REQUIRE(verify(*automaton, {"0"}, false));
  REQUIRE(verify(*automaton, {"1"}, true));
  REQUIRE(verify(*automaton, {"0", "0"}, false));
  REQUIRE(verify(*automaton, {"0", "1"}, false));
  REQUIRE(verify(*automaton, {"1", "0"}, true));
  REQUIRE(verify(*automaton, {"1", "1"}, true));
}

TEST_CASE("Translate <a & b>tt", "[translate]") {
  std::string formula_name = "<a & b>tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate <a | b>tt", "[translate]") {
  std::string formula_name = "<a | b>tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate {true}tt", "[translate]") {
  std::string formula_name = "[true]tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, true));
  REQUIRE(verify(*automaton, {{}}, true));
  REQUIRE(verify(*automaton, {{}, {}}, true));
}

TEST_CASE("Translate {a}tt", "[translate]") {
  std::string formula_name = "[a]tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, true));
  REQUIRE(verify(*automaton, {"0"}, true));
  REQUIRE(verify(*automaton, {"1"}, true));
  REQUIRE(verify(*automaton, {"0", "0"}, true));
  REQUIRE(verify(*automaton, {"0", "1"}, true));
  REQUIRE(verify(*automaton, {"1", "0"}, true));
  REQUIRE(verify(*automaton, {"1", "1"}, true));
}

TEST_CASE("Translate {a & b}ff", "[translate]") {
  std::string formula_name = "[a & b]ff";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate {a | b}ff", "[translate]") {
  std::string formula_name = "[a | b]ff";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate {a}ff", "[translate]") {
  std::string formula_name = "[a]ff";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, true));
  REQUIRE(verify(*automaton, {"0"}, true));
  REQUIRE(verify(*automaton, {"1"}, false));
  REQUIRE(verify(*automaton, {"0", "0"}, true));
  REQUIRE(verify(*automaton, {"0", "1"}, true));
  REQUIRE(verify(*automaton, {"1", "0"}, false));
  REQUIRE(verify(*automaton, {"1", "1"}, false));
}

TEST_CASE("Translate <<true>tt?>tt", "[translate]") {
  std::string formula_name = "<<true>tt?>tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

  REQUIRE(verify(*automaton, {}, false));
  REQUIRE(verify(*automaton, {{}}, true));
  REQUIRE(verify(*automaton, {{}, {}}, true));
}

TEST_CASE("Translate <{true}ff?>tt", "[translate]") {
  std::string formula_name = "<[true]ff?>tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);
  REQUIRE(verify(*automaton, {}, true));
  REQUIRE(verify(*automaton, {{}}, false));
  REQUIRE(verify(*automaton, {{}, {}}, false));
}

TEST_CASE("Translate <a plus b>tt", "[translate]") {
  std::string formula_name = "<a + b>tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);
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

TEST_CASE("Translate {a plus b}ff", "[translate]") {
  std::string formula_name = "[a + b]ff";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate <a,b>tt", "[translate]") {
  std::string formula_name = "<a ; b>tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate {a,b}ff", "[translate]") {
  std::string formula_name = "[a ; b]ff";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate <a*>tt", "[translate]") {
  std::string formula_name = "<a*>tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate {a*}tt", "[translate]") {
  std::string formula_name = "[a*]tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);
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

TEST_CASE("Translate <a*, b>tt", "[translate]") {
  std::string formula_name = "<a*; b>tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate [a*, b]ff", "[translate]") {
  std::string formula_name = "[a*; b]ff";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate <a* + b>tt", "[translate]") {
  std::string formula_name = "<a* + b>tt";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate [a* + b]ff", "[translate]") {
  std::string formula_name = "[a* + b]ff";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate <true*>(<a>tt & ~end)", "[translate]") {
  std::string formula_name = "<true*>(<a>tt & ~end)";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

TEST_CASE("Translate [true*](<a>tt | end)", "[translate]") {
  std::string formula_name = "[true*](<a>tt | end)";
  auto mgr = CUDD::Cudd();
  auto automaton = to_dfa_from_formula_string(formula_name, mgr);
  print_dfa(*automaton, formula_name);

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

} // namespace whitemech::lydia::Test