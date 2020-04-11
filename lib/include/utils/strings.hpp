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

#include <regex>
#include <string>
#include <vector>

namespace whitemech {
namespace lydia {

std::vector<std::string> split(const std::string &input,
                               const std::string &regex);

std::string to_upper_case(const std::string &);

bool strfind(std::string str, std::string target);

void print(std::vector<std::string> &v);

void print_int(std::vector<int> &v);

std::string state2bin(int n);

} // namespace lydia
} // namespace whitemech
