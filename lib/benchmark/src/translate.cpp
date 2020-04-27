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

#include "translate.hpp"
#include "logic.hpp"
#include "utils/benchmark.hpp"
#include <benchmark/benchmark.h>
#include <random>

namespace whitemech::lydia::Benchmark {

static void BM_translate_boolean(benchmark::State &state) {
  // we keep this outside since it's the operation that takes more time
  auto mgr = new CUDD::Cudd();
  auto x = LDLfBooleanAtom(true);
  for (auto _ : state) {
    auto my_dfa = to_dfa(x, mgr);
    escape(my_dfa);
    (void)my_dfa;
  }
}
// BENCHMARK(BM_translate_boolean);

static void BM_translate_diamond(benchmark::State &state) {
  // we keep this outside since it's the operation that takes more time
  auto mgr = new CUDD::Cudd();
  auto tt = std::make_shared<const LDLfBooleanAtom>(true);
  auto true_ = std::make_shared<const PropositionalTrue>();
  auto regex_true_ = std::make_shared<const PropositionalRegExp>(true_);
  auto diamond = LDLfDiamond(regex_true_, tt);
  for (auto _ : state) {
    auto my_dfa = to_dfa(diamond, mgr);
    escape(my_dfa);
    (void)my_dfa;
  }
}
// BENCHMARK(BM_translate_diamond);

} // namespace whitemech::lydia::Benchmark
