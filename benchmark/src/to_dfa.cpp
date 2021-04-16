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

#include <benchmark/benchmark.h>
#include <lydia/logic/ldlf/base.hpp>
#include <lydia/parser/ldlf/driver.hpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/strategies/bdd/base.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>
#include <lydia/to_dfa/strategies/naive.hpp>
#include <lydia/utils/benchmark.hpp>
#include <random>

namespace whitemech::lydia::Benchmark {

// Boolean atom
inline void translate_boolean(Strategy &s) {
  auto context = AstManager{};
  auto x = context.makeLdlfTrue();
  auto t = Translator(s);
  auto automaton = t.to_dfa(*x);
  escape(&automaton);
  (void)automaton;
}

template <class S> static void BM_translate_boolean(benchmark::State &state) {
  // we keep this outside since it's the operation that takes more time
  for (auto _ : state) {
    auto mgr =
        CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
    auto s = S(mgr);
    translate_boolean(s);
  }
}
BENCHMARK_TEMPLATE(BM_translate_boolean, NaiveStrategy);
BENCHMARK_TEMPLATE(BM_translate_boolean, BDDStrategy);

static void BM_translate_boolean_compositional(benchmark::State &state) {
  for (auto _ : state) {
    auto s = CompositionalStrategy();
    translate_boolean(s);
  }
}
BENCHMARK(BM_translate_boolean_compositional);

// Diamond
inline void translate_diamond(Strategy &s) {
  auto context = AstManager();
  auto tt = context.makeLdlfTrue();
  auto true_ = context.makeTrue();
  auto regex_true_ = context.makePropRegex(true_);
  auto diamond = context.makeLdlfDiamond(regex_true_, tt);
  auto t = Translator(s);
  auto automaton = t.to_dfa(*diamond);
  escape(&automaton);
  (void)automaton;
}

template <class S> static void BM_translate_diamond(benchmark::State &state) {
  // we keep this outside since it's the operation that takes more time
  for (auto _ : state) {
    auto mgr =
        CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
    auto s = S(mgr);
    translate_diamond(s);
  }
}
BENCHMARK_TEMPLATE(BM_translate_diamond, NaiveStrategy);
BENCHMARK_TEMPLATE(BM_translate_diamond, BDDStrategy);

static void BM_translate_diamond_compositional(benchmark::State &state) {
  for (auto _ : state) {
    auto s = CompositionalStrategy();
    translate_diamond(s);
  }
}
BENCHMARK(BM_translate_diamond_compositional);

// Sequence of atoms
inline void translate_sequence_of_atoms(int N, Strategy &s) {
  auto regex = sequence(N, ";");
  auto formula_string = "<" + regex + ">tt";
  auto sstream = std::stringstream(formula_string);
  auto driver = parsers::ldlf::Driver();
  driver.parse(sstream);
  auto formula = driver.result;
  auto translator = Translator(s);
  auto automaton = translator.to_dfa(*formula);
  escape(&automaton);
  (void)automaton;
}

static void BM_translate_sequence_of_atoms_bdd(benchmark::State &state) {
  for (auto _ : state) {
    auto mgr = CUDD::Cudd();
    auto s = BDDStrategy(mgr, 20);
    auto N = state.range(0);
    translate_sequence_of_atoms(N, s);
  }
}
// clang-format off
BENCHMARK(BM_translate_sequence_of_atoms_bdd)
  ->Arg(5)->Arg(10)->Arg(15)
  ->Arg(20)->Arg(25)->Arg(30)
  ->Arg(40)->Arg(80)->Arg(100)
  ->Arg(200)->Arg(500)
  ->Unit(benchmark::kMillisecond)
  ->Repetitions(5)
  ->DisplayAggregatesOnly(true);

static void BM_translate_sequence_of_atoms_compositional(benchmark::State &state) {
  for (auto _ : state) {
    auto s = CompositionalStrategy();
    auto N = state.range(0);
    translate_sequence_of_atoms(N, s);
  }
}
// clang-format off
BENCHMARK(BM_translate_sequence_of_atoms_compositional)
  ->Arg(5)->Arg(10)->Arg(15)
  ->Arg(20)->Arg(25)->Arg(30)
  ->Arg(40)
  ->Unit(benchmark::kMillisecond)
  ->Repetitions(5)
  ->DisplayAggregatesOnly(true);
// clang-format on

// Sequence of stars of atoms
inline void translate_sequence_of_stars_of_atoms(int N, Strategy &s) {
  auto regex = sequence(N, ";", true);
  auto formula_string = "<" + regex + ">tt";
  auto sstream = std::stringstream(formula_string);
  auto driver = parsers::ldlf::Driver();
  driver.parse(sstream);
  auto formula = driver.result;
  auto translator = Translator(s);
  auto automaton = translator.to_dfa(*formula);
  escape(&automaton);
  (void)automaton;
}

static void
BM_translate_sequence_of_stars_of_atoms_bdd(benchmark::State &state) {
  for (auto _ : state) {
    auto mgr =
        CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
    auto s = BDDStrategy(mgr, 20);
    auto N = state.range(0);
    translate_sequence_of_stars_of_atoms(N, s);
  }
}
// clang-format off
BENCHMARK(BM_translate_sequence_of_stars_of_atoms_bdd)
  ->Arg(5)->Arg(10)->Arg(15)
  ->Arg(20)->Arg(25)->Arg(30)
  ->Arg(40)->Arg(80)->Arg(100)
  ->Arg(200)->Arg(500)->Arg(1000)
  ->Unit(benchmark::kMillisecond)
  ->Repetitions(5)
  ->DisplayAggregatesOnly(true);

// clang-format on

static void
BM_translate_sequence_of_stars_of_atoms_compositional(benchmark::State &state) {
  for (auto _ : state) {
    auto s = CompositionalStrategy();
    auto N = state.range(0);
    translate_sequence_of_stars_of_atoms(N, s);
  }
}
// clang-format off
BENCHMARK(BM_translate_sequence_of_stars_of_atoms_compositional)
    ->Arg(5)->Arg(10)->Arg(15)
    ->Arg(20)->Arg(25)->Arg(30)
    ->Arg(40)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(5)
    ->DisplayAggregatesOnly(true);
// clang-format on

// Union
inline void translate_union(int N, Strategy &s) {
  auto regex = sequence(N, "+", false);
  auto formula_string = "<" + regex + ">tt";
  auto sstream = std::stringstream(formula_string);
  auto driver = parsers::ldlf::Driver();
  driver.parse(sstream);
  auto formula = driver.result;
  auto translator = Translator(s);
  auto automaton = translator.to_dfa(*formula);
  escape(&automaton);
  (void)automaton;
}

static void BM_translate_union_bdd(benchmark::State &state) {
  for (auto _ : state) {
    auto mgr =
        CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
    auto s = BDDStrategy(mgr, 20);
    auto N = state.range(0);
    translate_union(N, s);
  }
}
// clang-format off
  BENCHMARK(BM_translate_union_bdd)
      ->Arg(5)->Arg(10)->Arg(15)
      ->Arg(20)->Arg(25)->Arg(30)
      ->Arg(40)->Arg(80)->Arg(100)
      ->Arg(200)->Arg(500)
      ->Unit(benchmark::kMillisecond)
      ->Repetitions(5)
      ->DisplayAggregatesOnly(true);
// clang-format on

static void BM_translate_union_compositional(benchmark::State &state) {
  for (auto _ : state) {
    auto s = CompositionalStrategy();
    auto N = state.range(0);
    translate_union(N, s);
  }
}
// clang-format off
BENCHMARK(BM_translate_union_compositional)
    ->Arg(5)->Arg(10)->Arg(15)
    ->Arg(20)->Arg(25)->Arg(30)
    ->Arg(40)
    ->Unit(benchmark::kMillisecond)
    ->Repetitions(5)
    ->DisplayAggregatesOnly(true);
// clang-format on

} // namespace whitemech::lydia::Benchmark
