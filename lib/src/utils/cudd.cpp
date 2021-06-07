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

#include <algorithm>
#include <iostream>
#include <lydia/utils/cudd.hpp>

namespace whitemech::lydia {

std::vector<std::vector<uint8_t>> get_primes(const CUDD::BDD& f,
                                             int nb_variables) {
  std::vector<std::vector<uint8_t>> primes;
  int* cube = nullptr;
  DdGen* g = Cudd_FirstPrime(f.manager(), f.getNode(), f.getNode(), &cube);
  if (g != nullptr) {
    do {
      primes.emplace_back(std::vector<uint8_t>(cube, cube + nb_variables));
    } while (Cudd_NextPrime(g, &cube));
    Cudd_GenFree(g);
  }
  return primes;
}
std::vector<std::vector<uint8_t>> get_cubes(const CUDD::BDD& f,
                                            int nb_variables) {
  std::vector<std::vector<uint8_t>> cubes;
  int* cube = nullptr;
  CUDD_VALUE_TYPE value;
  DdGen* g = Cudd_FirstCube(f.manager(), f.getNode(), &cube, &value);
  if (g != nullptr) {
    do {
      cubes.emplace_back(std::vector<uint8_t>(cube, cube + nb_variables));
    } while (Cudd_NextCube(g, &cube, &value));
    Cudd_GenFree(g);
  }
  return cubes;
}

} // namespace whitemech::lydia
