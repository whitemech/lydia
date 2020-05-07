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

#include "pl/logic.hpp"
#include "pl/models_sat.hpp"
#include "utils/benchmark.hpp"
#include <benchmark/benchmark.h>
#include <random>

namespace whitemech::lydia::Benchmark {

static void BM_pl_models_cryptominisat_true(benchmark::State &state) {
  auto solver = CMSat::SATSolver();
  solver.set_num_threads(4);
  for (auto _ : state) {
    auto result = solver.solve();
    escape(&result);
    (void)result;
  }
}
BENCHMARK(BM_pl_models_cryptominisat_true);

static void BM_pl_models_cryptominisat_false(benchmark::State &state) {
  auto solver = CMSat::SATSolver();
  solver.set_num_threads(4);
  solver.add_clause(std::vector<CMSat::Lit>());
  for (auto _ : state) {
    auto result = solver.solve();
    escape(&result);
    (void)result;
  }
}
BENCHMARK(BM_pl_models_cryptominisat_false);

static void BM_pl_models_sat_all_models_t(benchmark::State &state) {
  auto t = boolean_prop(true);
  for (auto _ : state) {
    auto m = all_models_sat(*t);
    escape(&m);
    (void)m;
  }
}
BENCHMARK(BM_pl_models_sat_all_models_t);

} // namespace whitemech::lydia::Benchmark
