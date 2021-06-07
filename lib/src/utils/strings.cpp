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
#include <iterator>
#include <lydia/utils/strings.hpp>
#include <sstream>

namespace whitemech::lydia {

std::vector<std::string> split(const std::string& input,
                               const std::string& regex) {
  // passing -1 as the submatch index parameter performs splitting
  std::regex re(regex);
  std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last;
  return {first, last};
}

std::string to_upper_case(const std::string& s) {
  std::string result(s);
  std::transform(s.begin(), s.end(), result.begin(), ::toupper);
  return result;
}

bool strfind(std::string str, std::string target) {
  size_t found = str.find(target);
  return found != std::string::npos;
}

void print(std::vector<std::string>& v) {
  for (size_t n = 0; n < v.size(); n++)
    std::cout << v[n] << " ";
  std::cout << std::endl;
}

std::string print_vect_int(std::vector<int>& v) {
  std::string result = "{";
  for (size_t n = 0; n < v.size(); n++)
    result += std::to_string(v[n]) + ", ";
  result += "}";
  return result;
}

std::string print_vec(std::vector<item>& v) {
  std::string result = "{";
  for (size_t n = 0; n < v.size(); n++)
    result += print_vect_int(v[n]) + ", ";
  result += "}";
  return result;
}

/*!
 * Join a list of strings into a string.
 *
 * @param elements the list of elements
 * @param delim the delimiter
 * @return the joined string.
 */
std::string join(const std::vector<std::string>& elements, const char* delim) {
  std::ostringstream s;
  std::copy(elements.begin(), elements.end(),
            std::ostream_iterator<std::string>(s, delim));
  return s.str();
}

std::string state2bin(int n, int nb_fill_bits, bool increasing) {
  std::string res;
  while (n) {
    res.push_back((n & 1) + '0');
    n >>= 1;
  }

  auto current_length = res.length();
  if (nb_fill_bits != -1 && current_length < nb_fill_bits) {
    for (int i = 0; i < nb_fill_bits - current_length; i++) {
      res.push_back('0');
    }
  }

  if (res.empty())
    res = "0";

  if (not increasing)
    // reverse: from the most to the least significant bit.
    std::reverse(res.begin(), res.end());
  return res;
}

std::string interpretation2string(const interpretation& v) {
  std::stringstream ss;
  for (int i : v)
    ss << i;
  std::string s = ss.str();
  return s;
}

} // namespace whitemech::lydia
