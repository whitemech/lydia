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
#include <lydia/to_dfa/core.hpp>
#include <lydia/utils/benchmark.hpp>
#include <random>

namespace whitemech::lydia::Benchmark {

static void BM_cudd_manager_instantiation(benchmark::State &state) {
  for (auto _ : state) {
    auto mgr = CUDD::Cudd();
    escape(&mgr);
    (void)mgr;
  }
}
BENCHMARK(BM_cudd_manager_instantiation);

static void BM_dfa_instantiation(benchmark::State &state) {
  // we keep this outside since it's the operation that takes more time
  // preallocate
  auto mgr = CUDD::Cudd();
  for (auto _ : state) {
    auto my_dfa = dfa(mgr, 1, 1);
    escape(&my_dfa);
    (void)my_dfa;
  }
}
BENCHMARK(BM_dfa_instantiation);

} // namespace whitemech::lydia::Benchmark
