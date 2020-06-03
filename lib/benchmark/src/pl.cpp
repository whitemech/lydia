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
#include <lydia/pl/logic.hpp>
#include <lydia/pl/models/naive.hpp>
#include <lydia/pl/models/sat.hpp>
#include <lydia/pl/models_sat.hpp>
#include <lydia/utils/benchmark.hpp>
#include <minisat/simp/SimpSolver.h>
#include <random>

namespace whitemech::lydia::Benchmark {

static void BM_pl_models_minisat_1(benchmark::State &state) {
  for (auto _ : state) {
    Minisat::SimpSolver solver;
    solver.verbosity = -1;

    // Create variables
    auto A = solver.newVar();
    auto B = solver.newVar();
    auto C = solver.newVar();

    // Create the clauses
    solver.addClause(Minisat::mkLit(A), Minisat::mkLit(B), Minisat::mkLit(C));
    solver.addClause(~Minisat::mkLit(A), Minisat::mkLit(B), Minisat::mkLit(C));
    solver.addClause(Minisat::mkLit(A), ~Minisat::mkLit(B), Minisat::mkLit(C));
    solver.addClause(Minisat::mkLit(A), Minisat::mkLit(B), ~Minisat::mkLit(C));

    // Check for solution and retrieve model if found
    auto sat = solver.solve();

    escape((void *)&sat);
    (void)sat;
  }
}
BENCHMARK(BM_pl_models_minisat_1);

static void BM_pl_models_naive_all_models_tautology(benchmark::State &state) {
  auto t = boolean_prop(true);
  for (auto _ : state) {
    auto m = all_models<NaiveModelEnumerationStategy>(*t);
    escape(&m);
    (void)m;
  }
}
BENCHMARK(BM_pl_models_naive_all_models_tautology);

static void BM_pl_models_sat_all_models_tautology(benchmark::State &state) {
  auto t = boolean_prop(true);
  for (auto _ : state) {
    auto m = all_models<SATModelEnumerationStategy>(*t);
    escape(&m);
    (void)m;
  }
}
BENCHMARK(BM_pl_models_sat_all_models_tautology);

static void BM_pl_models_naive_all_models_or(benchmark::State &state) {
  auto t = boolean_prop(true);
  for (auto _ : state) {
    auto N = state.range(0);
    auto operands = std::vector<prop_ptr>();
    operands.reserve(N);
    for (int i = 0; i < N; i++) {
      operands.push_back(prop_atom(UniquePropositionalSymbol()));
    }
    auto f = logical_or(set_prop_formulas(operands.begin(), operands.end()));
    auto m = all_models<NaiveModelEnumerationStategy>(*f);
    assert(m.size() == 2 << N);
    escape(&m);
    (void)m;
  }
}
BENCHMARK(BM_pl_models_naive_all_models_or)->Arg(5)->Arg(10)->Arg(15);

static void BM_pl_models_sat_all_models_or(benchmark::State &state) {
  auto t = boolean_prop(true);
  for (auto _ : state) {
    auto N = state.range(0);
    auto operands = std::vector<prop_ptr>();
    operands.reserve(N);
    for (int i = 0; i < N; i++) {
      operands.push_back(prop_atom(UniquePropositionalSymbol()));
    }
    auto f = logical_or(set_prop_formulas(operands.begin(), operands.end()));
    auto m = all_models<SATModelEnumerationStategy>(*t);
    assert(m.size() == 2 << N);
    escape(&m);
    (void)m;
  }
}
BENCHMARK(BM_pl_models_sat_all_models_or)->Arg(5)->Arg(10)->Arg(20)->Arg(50);

} // namespace whitemech::lydia::Benchmark
