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
#include <lydia/utils/benchmark.hpp>
#include <random>

namespace whitemech::lydia::Benchmark {

static void BM_boolean_from_constructor(benchmark::State &state) {
  auto context = AstManager{};
  for (auto _ : state) {
    auto x = context.makeTrue();
    escape(&x);
    (void)x;
  }
}
BENCHMARK(BM_boolean_from_constructor);

static void BM_shared_boolean_from_constructor(benchmark::State &state) {
  auto context = AstManager{};
  for (auto _ : state) {
    auto x = context.makeTrue();
    escape(&x);
    (void)x;
  }
}
BENCHMARK(BM_shared_boolean_from_constructor);

static void BM_boolean_from_static_pointer(benchmark::State &state) {
  auto context = AstManager{};
  for (auto _ : state) {
    auto x = context.makeLdlfTrue();
    escape(&x);
    (void)x;
  }
}
BENCHMARK(BM_boolean_from_static_pointer);

static void BM_set_of_booleans_from_pointers(benchmark::State &state) {
  auto context = AstManager{};
  for (auto _ : state) {
    auto true_ = context.makeLdlfTrue();
    auto false_ = context.makeLdlfFalse();
    auto s = set_formulas{true_, false_};
    escape(&s);
    (void)s;
  }
}
BENCHMARK(BM_set_of_booleans_from_pointers);

static void BM_set_of_booleans_from_shared_pointers(benchmark::State &state) {
  auto context = AstManager{};
  for (auto _ : state) {
    auto true_ = context.makeLdlfTrue();
    auto false_ = context.makeLdlfFalse();
    auto s = set_formulas{true_, false_};
    escape(&s);
    (void)s;
  }
}
BENCHMARK(BM_set_of_booleans_from_shared_pointers);

static void BM_and_true_false(benchmark::State &state) {
  auto context = AstManager{};
  for (auto _ : state) {
    auto true_ = context.makeLdlfTrue();
    auto false_ = context.makeLdlfFalse();
    auto and_ = context.makeLdlfAnd(set_formulas{true_, false_});
    escape(&and_);
    (void)and_;
  }
}
BENCHMARK(BM_and_true_false);

static void BM_shared_and_true_false(benchmark::State &state) {
  auto context = AstManager{};
  for (auto _ : state) {
    auto true_ = context.makeLdlfTrue();
    auto false_ = context.makeLdlfFalse();
    auto and_ = context.makeLdlfAnd(set_formulas{true_, false_});
    escape(&and_);
    (void)and_;
  }
}
BENCHMARK(BM_shared_and_true_false);

} // namespace whitemech::lydia::Benchmark
