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

// This module contains helper functions.
// Some functions should be improved and generalized.
// They are inherited from other implementations.

#include <lydia/types.hpp>
#include <regex>
#include <string>
#include <vector>

namespace whitemech::lydia {

std::vector<std::string> split(const std::string& input,
                               const std::string& regex);

std::string to_upper_case(const std::string&);

bool strfind(std::string str, std::string target);

void print(std::vector<std::string>& v);

std::string print_vect_int(std::vector<int>& v);

std::string print_vec(std::vector<item>& v);

std::string join(const std::vector<std::string>& elements,
                 const char* delim = ", ");

/*!
 * Transform an integer in its binary representation.
 *
 * Optionally, you can provide the maximum number of bits
 * to fill.
 *
 * @param n the integer to transform.
 * @param nb_fill_bits the maximum number of MSBs to put to zero.
 * @param increasing: if true, from the least to the most significant bit.
 *                    If false, the opposite.
 * @return the binary representation of the integer.
 */
std::string state2bin(int n, int nb_fill_bits = -1, bool increasing = false);

/*
 * From interpretation to symbol
 */
std::string interpretation2string(const interpretation&);

} // namespace whitemech::lydia
