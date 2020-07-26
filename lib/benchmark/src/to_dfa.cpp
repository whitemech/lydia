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

#include <benchmark/benchmark.h>
#include <lydia/logic.hpp>
#include <lydia/parser/driver.hpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/strategies/bdd/base.hpp>
#include <lydia/to_dfa/strategies/naive.hpp>
#include <lydia/to_dfa/strategies/sat.hpp>
#include <lydia/utils/benchmark.hpp>
#include <random>

namespace whitemech::lydia::Benchmark {

template <class S> static void BM_translate_boolean(benchmark::State &state) {
  // we keep this outside since it's the operation that takes more time
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto x = LDLfBooleanAtom(true);
  for (auto _ : state) {
    auto s = S(mgr);
    auto t = Translator(s);
    auto automaton = t.to_dfa(x);
    escape(&automaton);
    (void)automaton;
  }
}
BENCHMARK_TEMPLATE(BM_translate_boolean, NaiveStrategy);
BENCHMARK_TEMPLATE(BM_translate_boolean, SATStrategy);
BENCHMARK_TEMPLATE(BM_translate_boolean, BDDStrategy);

template <class S> static void BM_translate_diamond(benchmark::State &state) {
  // we keep this outside since it's the operation that takes more time
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto tt = std::make_shared<const LDLfBooleanAtom>(true);
  auto true_ = std::make_shared<const PropositionalTrue>();
  auto regex_true_ = std::make_shared<const PropositionalRegExp>(true_);
  auto diamond = LDLfDiamond(regex_true_, tt);
  for (auto _ : state) {
    auto s = S(mgr);
    auto t = Translator(s);
    auto automaton = t.to_dfa(diamond);
    escape(&automaton);
    (void)automaton;
  }
}
BENCHMARK_TEMPLATE(BM_translate_diamond, NaiveStrategy);
BENCHMARK_TEMPLATE(BM_translate_diamond, SATStrategy);
BENCHMARK_TEMPLATE(BM_translate_diamond, BDDStrategy);

static void BM_translate_sequence_of_atoms(benchmark::State &state) {
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto sat_strategy = SATStrategy(mgr, 20);
  auto translator = Translator(sat_strategy);
  auto driver = Driver();
  for (auto _ : state) {
    auto N = state.range(0);
    auto regex = sequence(N, ";");
    auto formula_string = "<" + regex + ">tt";
    auto sstream = std::stringstream(formula_string);
    driver.parse(sstream);
    auto formula = driver.result;
    auto automaton = translator.to_dfa(*formula);
    escape(&automaton);
    (void)automaton;
  }
}
// clang-format off
  BENCHMARK(BM_translate_sequence_of_atoms)
      ->Arg(5)->Arg(10)->Arg(15)
      ->Arg(20)->Arg(25)->Arg(30)
      ->Arg(40)->Arg(80)->Arg(100)
      ->Arg(200)->Arg(500)->Arg(1000)
      ->Unit(benchmark::kMillisecond)
      ->Repetitions(5)
      ->DisplayAggregatesOnly(true);

  static void BM_translate_sequence_of_atoms_naive(benchmark::State &state) {
    auto mgr =
        CUDD::Cudd();
    auto strategy = BDDStrategy(mgr, 10);
    auto translator = Translator(strategy);
    auto driver = Driver();
    for (auto _ : state) {
      auto N = state.range(0);
      auto regex = sequence(N, ";");
      auto formula_string = "<" + regex + ">tt";
      auto sstream = std::stringstream(formula_string);
      driver.parse(sstream);
      auto formula = driver.result;
      auto automaton = translator.to_dfa(*formula);
      escape(&automaton);
      (void)automaton;
    }
  }
// clang-format off
  BENCHMARK(BM_translate_sequence_of_atoms_naive)
      ->Arg(1)
      ->Arg(2)->Arg(3)->Arg(4)->Arg(5)
      ->Arg(6)->Arg(7)->Arg(8)->Arg(9)->Arg(10)
      ->Arg(11)->Arg(12)
      ->Unit(benchmark::kMillisecond)
      ->Repetitions(5)
      ->DisplayAggregatesOnly(true);

// clang-format on

static void BM_translate_sequence_of_stars_of_atoms(benchmark::State &state) {
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto sat_strategy = SATStrategy(mgr, 20);
  auto translator = Translator(sat_strategy);
  auto driver = Driver();
  for (auto _ : state) {
    auto N = state.range(0);
    auto regex = sequence(N, ";", true);
    auto formula_string = "<" + regex + ">tt";
    auto sstream = std::stringstream(formula_string);
    driver.parse(sstream);
    auto formula = driver.result;
    auto automaton = translator.to_dfa(*formula);
    escape(&automaton);
    (void)automaton;
  }
}
// clang-format off
  BENCHMARK(BM_translate_sequence_of_stars_of_atoms)
      ->Arg(5)->Arg(10)->Arg(15)
      ->Arg(20)->Arg(25)->Arg(30)
      ->Arg(40)->Arg(80)->Arg(100)
      ->Arg(200)->Arg(500)->Arg(1000)
      ->Unit(benchmark::kMillisecond)
      ->Repetitions(5)
      ->DisplayAggregatesOnly(true);
// clang-format on

static void
BM_translate_sequence_of_stars_of_atoms_naive(benchmark::State &state) {
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto sat_strategy = BDDStrategy(mgr, 20);
  auto translator = Translator(sat_strategy);
  auto driver = Driver();
  for (auto _ : state) {
    auto N = state.range(0);
    auto regex = sequence(N, ";", true);
    auto formula_string = "<" + regex + ">tt";
    auto sstream = std::stringstream(formula_string);
    driver.parse(sstream);
    auto formula = driver.result;
    auto automaton = translator.to_dfa(*formula);
    escape(&automaton);
    (void)automaton;
  }
}
// clang-format off
  BENCHMARK(BM_translate_sequence_of_stars_of_atoms_naive)
      ->Arg(5)->Arg(10)->Arg(15)
      ->Arg(20)->Arg(25)->Arg(30)
      ->Arg(40)->Arg(80)->Arg(100)
      ->Arg(200)->Arg(500)->Arg(1000)
      ->Unit(benchmark::kMillisecond)
      ->Repetitions(5)
      ->DisplayAggregatesOnly(true);

// clang-format on

static void BM_translate_union(benchmark::State &state) {
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto sat_strategy = SATStrategy(mgr, 30);
  auto translator = Translator(sat_strategy);
  auto driver = Driver();
  for (auto _ : state) {
    auto N = state.range(0);
    auto regex = sequence(N, "+", false);
    auto formula_string = "<" + regex + ">tt";
    auto sstream = std::stringstream(formula_string);
    driver.parse(sstream);
    auto formula = driver.result;
    auto automaton = translator.to_dfa(*formula);
    escape(&automaton);
    (void)automaton;
  }
}
// clang-format off
  BENCHMARK(BM_translate_union)
      ->Arg(1)->Arg(2)->Arg(3)
      ->Arg(4)->Arg(5)->Arg(6)
      ->Arg(7)->Arg(8)
      ->Unit(benchmark::kMillisecond)
      ->Repetitions(5)
      ->DisplayAggregatesOnly(true);
// clang-format on

static void BM_translate_union_naive(benchmark::State &state) {
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto sat_strategy = BDDStrategy(mgr, 30);
  auto translator = Translator(sat_strategy);
  auto driver = Driver();
  for (auto _ : state) {
    auto N = state.range(0);
    auto regex = sequence(N, "+", false);
    auto formula_string = "<" + regex + ">tt";
    auto sstream = std::stringstream(formula_string);
    driver.parse(sstream);
    auto formula = driver.result;
    auto automaton = translator.to_dfa(*formula);
    escape(&automaton);
    (void)automaton;
  }
}
// clang-format off
  BENCHMARK(BM_translate_union_naive)
      ->Arg(1)->Arg(2)->Arg(3)
      ->Arg(4)->Arg(5)->Arg(6)
      ->Arg(7)->Arg(8)->Arg(9)
      ->Arg(10)->Arg(11)->Arg(12)
      ->Arg(13)
      ->Unit(benchmark::kMillisecond)
      ->Repetitions(5)
      ->DisplayAggregatesOnly(true);
// clang-format on

} // namespace whitemech::lydia::Benchmark
