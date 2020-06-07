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
#include <lydia/to_dfa/strategies/sat.hpp>
#include <lydia/utils/benchmark.hpp>
#include <random>

namespace whitemech::lydia::Benchmark {

static void BM_translate_boolean(benchmark::State &state) {
  // we keep this outside since it's the operation that takes more time
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto x = LDLfBooleanAtom(true);
  for (auto _ : state) {
    auto automaton = to_dfa(x, mgr);
    escape(&automaton);
    (void)automaton;
  }
}
BENCHMARK(BM_translate_boolean);

static void BM_translate_diamond(benchmark::State &state) {
  // we keep this outside since it's the operation that takes more time
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto tt = std::make_shared<const LDLfBooleanAtom>(true);
  auto true_ = std::make_shared<const PropositionalTrue>();
  auto regex_true_ = std::make_shared<const PropositionalRegExp>(true_);
  auto diamond = LDLfDiamond(regex_true_, tt);
  for (auto _ : state) {
    auto automaton = to_dfa(diamond, mgr);
    escape(&automaton);
    (void)automaton;
  }
}
BENCHMARK(BM_translate_diamond);

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

static void BM_translate_union(benchmark::State &state) {
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto sat_strategy = SATStrategy(mgr, 20);
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
->Arg(5)->Arg(10)->Arg(15)
->Arg(20)->Arg(25)->Arg(30)
->Arg(40)->Arg(80)->Arg(100)
->Arg(200)->Arg(500)->Arg(1000)
->Unit(benchmark::kMillisecond)
->Repetitions(5)
->DisplayAggregatesOnly(true);
// clang-format on

} // namespace whitemech::lydia::Benchmark
