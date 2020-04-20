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

#include <set>
#include <cmath>
#include <types.hpp>

namespace whitemech {
namespace lydia {

template <typename T, typename U = std::less<T>>
std::vector<std::set<T, U>> powerset(std::set<T, U> &s) {
  /*TODO this method only works for a set of elements
   * whose size is lower than 64. Handle those cases.
   */
  assert(s.size() < 64);
  std::vector<std::set<T, U>> result;
  auto vect = std::vector<T>(s.begin(), s.end());
  size_t size = s.size();

  // we interpret the integer 'i' as a bit string
  // that defines membership to the ith subset, for each element.
  // the digit at jth position means "if 1, the jth element
  // belongs to the ith subset".
  for (int i = 0; i < std::pow(2, size); i++) {
    std::set<T, U> tmp;
    u_int64_t mask = i;
    int index = 0;
    while (mask) {
      if (mask % 2 != 0) {
        tmp.insert(vect[index]);
      }
      mask >>= 1;
      ++index;
    }
    result.emplace_back(tmp);
  }
  return result;
}

unsigned inline bit_length(int x) {
  unsigned bits, var = (x < 0) ? -x : x;
  for (bits = 0; var != 0; ++bits)
    var >>= 1;
  return bits;
}

} // namespace lydia
} // namespace whitemech
