#pragma once
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

#include <memory>
#include <type_traits>

#include <lydia/logic/ldlf/base.hpp>
#include <lydia/logic/pl/base.hpp>
#include <lydia/types.hpp>

static void escape(void* p) { asm volatile("" : : "g"(p) : "memory"); }

static void clobber() { asm volatile("" : : : "memory"); }

static const int BENCH_CUDD_UNIQUE_SLOTS = CUDD_UNIQUE_SLOTS;
static const int BENCH_CUDD_CACHE_SLOTS = CUDD_CACHE_SLOTS;

namespace whitemech::lydia {

// function generator:
inline std::string propositional(int i) { return "p_" + std::to_string(i); }

// class generator:
static struct c_unique {
  int current;
  c_unique() { current = 0; }
  std::string operator()() { return propositional(current++); }
} UniquePropositionalSymbol;

static std::string sequence(const int N, const std::string& op,
                            bool with_star = false) {
  auto postfix = with_star ? "*" : "";
  std::string result = UniquePropositionalSymbol() + postfix;
  for (int i = 1; i < N; i++) {
    result += " " + op + " " + UniquePropositionalSymbol() + postfix;
  }
  return result;
}

} // namespace whitemech::lydia