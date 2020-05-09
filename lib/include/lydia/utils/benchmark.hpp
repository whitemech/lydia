#pragma once
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

#include <lydia/types.hpp>
#include <memory>
#include <type_traits>

static void escape(void *p) { asm volatile("" : : "g"(p) : "memory"); }

static void clobber() { asm volatile("" : : : "memory"); }

static const int BENCH_CUDD_UNIQUE_SLOTS = CUDD_UNIQUE_SLOTS;
static const int BENCH_CUDD_CACHE_SLOTS = CUDD_CACHE_SLOTS;

namespace whitemech::lydia {

// function generator:
std::string propositional(int i) { return "p_" + std::to_string(i); }

// class generator:
struct c_unique {
  int current;
  c_unique() { current = 0; }
  std::string operator()() { return propositional(current++); }
} UniquePropositionalSymbol;

static std::shared_ptr<SequenceRegExp> intitialize_sequence_regex(const int N) {
  auto vregex = vec_regex();
  vregex.reserve(N);
  for (int i = 0; i < N; i++) {
    auto tmp =
        std::make_shared<const PropositionalAtom>(UniquePropositionalSymbol());
    auto tmp_ptr = std::make_shared<const PropositionalRegExp>(tmp);
    vregex.push_back(tmp_ptr);
  }
  return std::make_shared<SequenceRegExp>(vregex);
}

static std::shared_ptr<UnionRegExp> intitialize_union_regex(const int N) {
  auto sregex = set_regex();
  for (int i = 0; i < N; i++) {
    auto tmp =
        std::make_shared<const PropositionalAtom>(UniquePropositionalSymbol());
    auto tmp_ptr = std::make_shared<const PropositionalRegExp>(tmp);
    sregex.insert(tmp_ptr);
  }
  return std::make_shared<UnionRegExp>(sregex);
}

} // namespace whitemech::lydia