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

#include <cuddObj.hh>

namespace whitemech::lydia {

std::vector<std::vector<uint8_t>> get_primes(const CUDD::BDD& f,
                                             int nb_variables);
std::vector<std::vector<uint8_t>> get_cubes(const CUDD::BDD& f,
                                            int nb_variables);

} // namespace whitemech::lydia