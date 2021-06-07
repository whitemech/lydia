#pragma once
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
#include <bitset>
#include <catch.hpp>
#include <cppitertools/powerset.hpp>
#include <cppitertools/product.hpp>
#include <cuddObj.hh>
#include <numeric>
#include <sstream>
#include <stack>

#include <filesystem>
#include <fstream>
#include <lydia/dfa/dfa.hpp>
#include <lydia/logic/ldlf/base.hpp>
#include <lydia/parser/ldlf/driver.hpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/strategies/bdd/base.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>
#include <lydia/to_dfa/strategies/naive.hpp>

namespace whitemech::lydia {

template <class Tuple,
          class T = std::decay_t<std::tuple_element_t<0, std::decay_t<Tuple>>>>
std::vector<T> to_vector(Tuple&& tuple) {
  return std::apply(
      [](auto&&... elems) {
        return std::vector<T>{std::forward<decltype(elems)>(elems)...};
      },
      std::forward<Tuple>(tuple));
}

// function generator:
inline std::string propositional(int i) { return "p_" + std::to_string(i); }

static trace to_trace(const std::vector<std::string>& trace_) {
  trace t;
  t.reserve(trace_.size());
  for (const auto& s : trace_) {
    interpretation p;
    for (int i = s.size() - 1; i >= 0; --i) {
      p.push_back(s[i] - 48);
    }
    t.push_back(p);
  }
  return t;
}

static bool verify(const abstract_dfa& automaton,
                   const std::vector<std::string>& trace_, bool expected) {
  return automaton.accepts(to_trace(trace_)) == expected;
}

typedef bool (*boolean_condition)(bool a, bool b);
static boolean_condition equal = [](bool a, bool b) { return a == b; };
static boolean_condition not_equal = [](bool a, bool b) { return a != b; };

static interpretation from_interpretation_set(const interpretation_set& s,
                                              int nb_prop) {
  interpretation result(nb_prop);
  result.reserve(nb_prop);
  std::vector<int> tmp(s.begin(), s.end());
  int current_idx;
  for (current_idx = 0; current_idx < tmp.size();) {
    int next_true_value_idx = tmp[current_idx];
    for (int j = current_idx; j < next_true_value_idx; j++) {
      result.push_back(0);
    }
    result.push_back(1);
    current_idx = next_true_value_idx + 1;
  }
  for (int i = current_idx; i < nb_prop; i++)
    result[i] = 0;

  return result;
}

static std::vector<interpretation>
from_trace_set(std::vector<interpretation_set> vector, int prop);

template <int length>
static bool compare(
    const abstract_dfa& automaton_1, const abstract_dfa& automaton_2,
    int nb_prop, boolean_condition bc = [](bool a, bool b) { return a == b; }) {
  if (automaton_1.get_nb_variables() != automaton_2.get_nb_variables())
    return false;

  std::vector<int> full_interpretation;
  full_interpretation.reserve(nb_prop);
  std::iota(full_interpretation.begin(), full_interpretation.end(), 0);
  const auto& powerset = iter::powerset(full_interpretation);
  std::vector<interpretation_set> all_interpretations;
  for (auto&& st : powerset) {
    all_interpretations.emplace_back(std::begin(st), std::end(st));
  }

  for (const auto& trace_ : iter::product<length>(all_interpretations)) {
    std::vector<interpretation_set> current_trace_set = to_vector(trace_);
    std::vector<interpretation> current_trace =
        from_trace_set(current_trace_set, nb_prop);
    bool first_accepts = automaton_1.accepts(current_trace);
    bool second_accepts = automaton_2.accepts(current_trace);
    if (!bc(first_accepts, second_accepts))
      return false;
  }
  return true;
}

static std::vector<interpretation>
from_trace_set(std::vector<interpretation_set> vector, int prop) {
  std::vector<interpretation> result;
  result.reserve(vector.size());
  for (const auto& v : vector) {
    result.push_back(from_interpretation_set(v, prop));
  }
  return result;
}

template <typename Parser = parsers::ldlf::Driver>
static adfa_ptr to_dfa_from_formula_string(const std::string& f, Strategy& s) {
  auto driver = Parser();
  std::stringstream ldlf_formula_stream(f);
  driver.parse(ldlf_formula_stream);
  const auto& formula = *driver.result;
  auto result = to_dfa_with_strategy(formula, s);
  return result;
}

static adfa_ptr to_dfa_from_formula_file(const std::filesystem::path& path,
                                         Strategy& s) {
  std::ifstream t(path.string());
  std::stringstream buffer;
  buffer << t.rdbuf();
  return to_dfa_from_formula_string(buffer.str(), s);
}

struct StrategyGenerator
    : public Catch::Generators::IGenerator<
          std::function<std::shared_ptr<Strategy>(const CUDD::Cudd&)>> {
public:
  int i = 0;
  std::vector<std::function<std::shared_ptr<Strategy>(const CUDD::Cudd&)>>
      factories;
  StrategyGenerator();
  bool next() override;
  [[nodiscard]] std::function<
      std::shared_ptr<Strategy>(const CUDD::Cudd&)> const&
  get() const override;

  static std::shared_ptr<Strategy> make_compositional(const CUDD::Cudd& mgr) {
    return std::make_shared<CompositionalStrategy>();
  }
  static std::shared_ptr<Strategy> make_bdd(const CUDD::Cudd& mgr) {
    return std::make_shared<BDDStrategy>(mgr, 20);
  }
  static std::shared_ptr<Strategy> make_naive(const CUDD::Cudd& mgr) {
    return std::make_shared<NaiveStrategy>(CUDD::Cudd(), 20);
  }
};
Catch::Generators::GeneratorWrapper<
    std::function<std::shared_ptr<Strategy>(const CUDD::Cudd&)>>
strategies();

} // namespace whitemech::lydia