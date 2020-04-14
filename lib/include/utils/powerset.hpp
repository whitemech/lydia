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
namespace whitemech{
namespace lydia{

template <typename T>
std::set<std::set<T>> powerset(std::set<T>& s){
  /*TODO this method only works for a set of elements
   * whose size is lower than 64. Handle those cases.
  */
  assert(s.size() < 64);
  std::set<std::set<T>> result;
  auto vect = std::vector<T>(s.begin(), s.end());
  size_t size = s.size();

  // we interpret the integer 'i' as a bit string
  // that defines membership to the ith subset, for each element.
  // the digit at jth position means "if 1, the jth element
  // belongs to the ith subset".
  for (int i = 0; i < std::pow(2, size); i++){
    std::set<T> tmp;
    u_int64_t mask = i;
    int index = 0;
    while (mask){
      if(mask % 2 != 0){
        tmp.insert(vect[index]);
      }
      mask>>=1;
      ++index;
    }
    result.insert(tmp);
  }
  return result;
}

}}

