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
#include <lydia/logic/pl/base.hpp>
#include <lydia/logic/pl/models/naive.hpp>
#include <lydia/utils/benchmark.hpp>
#include <random>

namespace whitemech::lydia::Benchmark {

static void BM_pl_models_naive_all_models_tautology(benchmark::State &state) {
  auto context = AstManager{};
  auto t = context.makeTrue();
  for (auto _ : state) {
    auto m = all_models<NaiveModelEnumerationStategy>(*t);
    escape(&m);
    (void)m;
  }
}
BENCHMARK(BM_pl_models_naive_all_models_tautology);

static void BM_pl_models_naive_all_models_or(benchmark::State &state) {
  auto context = AstManager{};
  auto t = context.makeBool(true);
  for (auto _ : state) {
    auto N = state.range(0);
    auto operands = std::vector<prop_ptr>();
    operands.reserve(N);
    for (int i = 0; i < N; i++) {
      operands.push_back(context.makePropAtom(UniquePropositionalSymbol()));
    }
    auto f = context.makePropOr(set_prop_formulas(operands.begin(), operands.end()));
    auto m = all_models<NaiveModelEnumerationStategy>(*f);
    assert(m.size() == ((1 << N) - 1));
    escape(&m);
    (void)m;
  }
}
BENCHMARK(BM_pl_models_naive_all_models_or)->Arg(5)->Arg(10)->Arg(15);

} // namespace whitemech::lydia::Benchmark
