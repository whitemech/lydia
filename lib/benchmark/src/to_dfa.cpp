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
#include <lydia/to_dfa/core.hpp>
#include <lydia/utils/benchmark.hpp>
#include <random>

namespace whitemech::lydia::Benchmark {

static void BM_translate_boolean(benchmark::State &state) {
  // we keep this outside since it's the operation that takes more time
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto x = LDLfBooleanAtom(true);
  for (auto _ : state) {
    auto my_dfa = to_dfa(x, mgr);
    escape(&my_dfa);
    (void)my_dfa;
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
    auto my_dfa = to_dfa(diamond, mgr);
    escape(&my_dfa);
    (void)my_dfa;
  }
}
BENCHMARK(BM_translate_diamond);

static void BM_translate_sequence(benchmark::State &state) {
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto tt = boolean(true);
  for (auto _ : state) {
    auto N = state.range(0);
    auto regex = intitialize_sequence_regex(N);
    auto diamond_formula = std::make_shared<LDLfDiamond>(regex, tt);
    auto my_dfa = to_dfa(*diamond_formula, mgr);
    escape(&my_dfa);
    (void)my_dfa;
  }
}
// clang-format off
BENCHMARK(BM_translate_sequence)->Arg(2)->Arg(3)->Arg(4)
                                ->Arg(5)->Arg(6)->Arg(7)
                                ->Arg(8)->Arg(9)->Arg(10)
                                ->Arg(11)->Arg(12)->Arg(13)
                                ->Unit(benchmark::kMillisecond)
                                ->Repetitions(5)
                                ->DisplayAggregatesOnly(true);
// clang-format on

static void BM_translate_union(benchmark::State &state) {
  auto mgr =
      CUDD::Cudd(0, 0, BENCH_CUDD_UNIQUE_SLOTS, BENCH_CUDD_CACHE_SLOTS, 0);
  auto tt = boolean(true);
  for (auto _ : state) {
    auto N = state.range(0);
    auto regex = intitialize_union_regex(N);
    auto diamond_formula = std::make_shared<LDLfDiamond>(regex, tt);
    auto my_dfa = to_dfa(*diamond_formula, mgr);
    escape(&my_dfa);
    (void)my_dfa;
  }
}
// clang-format off
BENCHMARK(BM_translate_union)->Arg(2)->Arg(3)->Arg(4)
                             ->Arg(5)->Arg(6)->Arg(7)
                             ->Arg(8)->Arg(9)->Arg(10)
                             ->Arg(11)->Arg(12)->Arg(13)
                             ->Unit(benchmark::kMillisecond)
                             ->Repetitions(5)
                             ->DisplayAggregatesOnly(true);
// clang-format on

} // namespace whitemech::lydia::Benchmark
