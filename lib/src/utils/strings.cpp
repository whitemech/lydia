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

#include "utils/strings.hpp"
#include <algorithm>
#include <iostream>

namespace whitemech {
namespace lydia {

std::vector<std::string> split(const std::string &input,
                               const std::string &regex) {
  // passing -1 as the submatch index parameter performs splitting
  std::regex re(regex);
  std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;
  return {first, last};
}

std::string to_upper_case(const std::string &s) {
  std::string result(s);
  std::transform(s.begin(), s.end(), result.begin(), ::toupper);
  return result;
}

bool strfind(std::string str, std::string target) {
  size_t found = str.find(target);
  return found != std::string::npos;
}

void print(std::vector<std::string> &v) {
  for (size_t n = 0; n < v.size(); n++)
    std::cout << v[n] << " ";
  std::cout << std::endl;
}

void print_int(std::vector<int> &v) {
  for (size_t n = 0; n < v.size(); n++)
    std::cout << v[n] << " ";
  std::cout << std::endl;
}

std::string state2bin(int n) {
  std::string res;
  while (n) {
    res.push_back((n & 1) + '0');
    n >>= 1;
  }

  if (res.empty())
    res = "0";
  else
    std::reverse(res.begin(), res.end());
  // std::cout<<res<<std::endl;
  return res;
}

} // namespace lydia
} // namespace whitemech
