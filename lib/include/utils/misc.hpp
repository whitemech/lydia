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

#include "strings.hpp"
#include <cassert>
#include <set>
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
  for (int i = 0; i < (1 << size); i++) {
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

int inline bin2state(const std::vector<int> &v) {
  int size = v.size();
  int result = v[0];
  for (int i = 1; i < size; ++i) {
    result *= 2;
    result += v[i] % 2;
  }
  return result;
}
int inline bin2state(const std::string &s) {
  int size = s.size();
  int result = s[0] - '0';
  for (int i = 1; i < size; ++i) {
    result *= 2;
    result += s[i] - '0';
  }
  return result;
}

std::vector<int> inline state2binvec(int n, int nb_fill_bits) {
  auto bin_string = state2bin(n, nb_fill_bits);
  std::vector<int> result(bin_string.size(), 0);
  int i = 0;
  for (const char &c : bin_string) {
    result[i] = c - '0';
    ++i;
  }
  return result;
}

} // namespace lydia
} // namespace whitemech
