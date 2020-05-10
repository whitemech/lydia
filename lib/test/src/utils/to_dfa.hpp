#pragma once
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

#include <bitset>
#include <cuddObj.hh>
#include <lydia/dfa.hpp>
#include <lydia/logic.hpp>
#include <lydia/parser/driver.hpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/utils/dfa_transform.hpp>
#include <numeric>
#include <powerset.hpp>
#include <product.hpp>
#include <sstream>
#include <stack>

namespace whitemech {
namespace lydia {

template <class Tuple,
          class T = std::decay_t<std::tuple_element_t<0, std::decay_t<Tuple>>>>
std::vector<T> to_vector(Tuple &&tuple) {
  return std::apply(
      [](auto &&... elems) {
        return std::vector<T>{std::forward<decltype(elems)>(elems)...};
      },
      std::forward<Tuple>(tuple));
}

// function generator:
inline std::string propositional(int i) { return "p_" + std::to_string(i); }

static trace to_trace(const std::vector<std::string> &trace_) {
  trace t;
  t.reserve(trace_.size());
  for (const auto &s : trace_) {
    interpretation p;
    for (int i = s.size() - 1; i >= 0; --i) {
      p.push_back(s[i] - 48);
    }
    t.push_back(p);
  }
  return t;
}

static bool verify(const dfa &automaton, const std::vector<std::string> &trace_,
                   bool expected) {
  return automaton.accepts(to_trace(trace_)) == expected;
}

typedef bool (*boolean_condition)(bool a, bool b);
static boolean_condition equal = [](bool a, bool b) { return a == b; };
static boolean_condition not_equal = [](bool a, bool b) { return a != b; };

static interpretation from_interpretation_set(const interpretation_set &s,
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
    const dfa &automaton_1, const dfa &automaton_2, int nb_prop,
    boolean_condition bc = [](bool a, bool b) { return a == b; }) {
  if (automaton_1.nb_variables != automaton_2.nb_variables)
    return false;

  std::vector<int> full_interpretation;
  full_interpretation.reserve(nb_prop);
  std::iota(full_interpretation.begin(), full_interpretation.end(), 0);
  const auto &powerset = iter::powerset(full_interpretation);
  std::vector<interpretation_set> all_interpretations;
  for (auto &&st : powerset) {
    all_interpretations.emplace_back(std::begin(st), std::end(st));
  }

  for (const auto &trace_ : iter::product<length>(all_interpretations)) {
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
  for (const auto &v : vector) {
    result.push_back(from_interpretation_set(v, prop));
  }
  return result;
}

static dfa_ptr to_dfa_from_formula_string(const std::string &f,
                                          const CUDD::Cudd &mgr) {
  auto driver = Driver();
  std::stringstream ldlf_formula_stream(f);
  driver.parse(ldlf_formula_stream);
  const auto &formula = *driver.result;
  return to_dfa(formula, mgr);
}

static void print_dfa(const dfa &automaton, const std::string &name,
                      const std::string &format = "svg") {
  dfa_to_graphviz(automaton, name + "." + format, format);
}

} // namespace lydia
} // namespace whitemech