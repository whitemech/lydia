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

#include "acceptance_test.hpp"

namespace whitemech {
namespace lydia {

bool verify(const dfa &automaton, const std::vector<std::string> &trace_,
            bool expected) {
  trace t;
  t.reserve(trace_.size());
  for (const auto &s : trace_) {
    interpretation p;
    for (int i = s.size() - 1; i >= 0; --i) {
      p.push_back(s[i] - 48);
    }
    t.push_back(p);
  }
  return automaton.accepts(t) == expected;
}

} // namespace lydia
} // namespace whitemech