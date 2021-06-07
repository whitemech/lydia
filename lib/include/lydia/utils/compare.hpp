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

#include <algorithm>
#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace whitemech::lydia {

class Basic;
class LDLfFormula;

// Takes an unordered map of type M with key type K and returns a vector of K
// ordered by C.
template <class M, typename C = std::less<typename M::key_type>>
std::vector<typename M::key_type> sorted_keys(const M& d) {
  std::vector<typename M::key_type> v;
  v.reserve(d.size());
  for (auto& p : d) {
    v.push_back(p.first);
  }
  std::sort(v.begin(), v.end(), C());
  return v;
}

// template <bool B, class T = void>
// using enable_if_t = typename std::enable_if<B, T>::type;

template <typename T, typename U>
inline bool unified_eq(const std::pair<T, U>& a, const std::pair<T, U>& b) {
  return unified_eq(a.first, b.first) and unified_eq(a.second, b.second);
}

template <typename T, typename U>
inline bool unified_eq(const std::set<T, U>& a, const std::set<T, U>& b) {
  return ordered_eq(a, b);
}

template <typename T, typename U>
inline bool unified_eq(const std::multiset<T, U>& a,
                       const std::multiset<T, U>& b) {
  return ordered_eq(a, b);
}

template <typename K, typename V, typename C>
inline bool unified_eq(const std::map<K, V, C>& a, const std::map<K, V, C>& b) {
  return ordered_eq(a, b);
}

template <typename K, typename V, typename H, typename E>
inline bool unified_eq(const std::unordered_map<K, V, H, E>& a,
                       const std::unordered_map<K, V, H, E>& b) {
  return unordered_eq(a, b);
}

template <typename T>
inline bool unified_eq(const std::shared_ptr<T>& a,
                       const std::shared_ptr<T>& b) {
  return unified_eq(*a, *b);
}

// template <typename T, typename U,
//          typename = enable_if_t<std::is_base_of<Basic, T>::value and
//                                 std::is_base_of<Basic, U>::value>>
template <typename T, typename U>
inline bool unified_eq(const T& a, const U& b) {
  return eq(*a, *b);
}

// template <typename T, typename = enable_if_t<std::is_arithmetic<T>::value>>
template <typename T> inline bool unified_eq(const T& a, const T& b) {
  return a == b;
}

//! eq function base
//! \return true if the two dictionaries `a` and `b` are equal. Otherwise false
template <class T> inline bool unordered_eq(const T& a, const T& b) {
  // This follows the same algorithm as Python's dictionary comparison
  // (a==b), which is implemented by "dict_equal" function in
  // Objects/dictobject.c.

  // Can't be equal if # of entries differ:
  if (a.size() != b.size())
    return false;
  // Loop over keys in "a":
  for (const auto& p : a) {
    // O(1) lookup of the key in "b":
    auto f = b.find(p.first);
    if (f == b.end())
      return false; // no such element in "b"
    if (not unified_eq(p.second, f->second))
      return false; // values not equal
  }
  return true;
}

template <class T> inline bool ordered_eq(const T& A, const T& B) {
  // Can't be equal if # of entries differ:
  if (A.size() != B.size())
    return false;
  // Loop over elements in "a" and "b":
  auto a = A.begin();
  auto b = B.begin();
  for (; a != A.end(); ++a, ++b) {
    if (not unified_eq(*a, *b))
      return false; // values not equal
  }
  return true;
}

template <typename T>
inline bool unified_eq(const std::vector<T>& a, const std::vector<T>& b) {
  return ordered_eq(a, b);
}

//! compare functions base
//! \return -1, 0, 1 for a < b, a == b, a > b
// template <typename T, typename = enable_if_t<std::is_arithmetic<T>::value>>
template <typename T> inline int unified_compare(const T& a, const T& b) {
  if (a == b)
    return 0;
  return a < b ? -1 : 1;
}

template <typename T>
inline int unified_compare(const std::shared_ptr<T>& a,
                           const std::shared_ptr<T>& b) {
  return unified_compare(*a, *b);
}

// template <typename T, typename U,
//          typename = enable_if_t<std::is_base_of<Basic, T>::value and
//                                 std::is_base_of<Basic, U>::value>>
template <typename T, typename U>
inline int unified_compare(const T& a, const U& b) {
  return a->compare(*b);
}

template <typename T>
inline int unified_compare(const std::vector<T>& a, const std::vector<T>& b) {
  return ordered_compare(a, b);
}

template <typename T, typename U>
inline int unified_compare(const std::set<T, U>& a, const std::set<T, U>& b) {
  // TODO try to replace this with native comparison: a < b
  return ordered_compare(a, b);
}

template <typename T, typename U>
inline int unified_compare(const std::multiset<T, U>& a,
                           const std::multiset<T, U>& b) {
  return ordered_compare(a, b);
}

template <typename T, typename U>
inline int unified_compare(const std::pair<T, U>& a, const std::pair<T, U>& b) {
  auto t = unified_compare(a.first, b.first);
  if (t == 0) {
    return unified_compare(a.second, b.second);
  } else {
    return t;
  }
}

template <typename K, typename V, typename C>
inline int unified_compare(const std::map<K, V, C>& a,
                           const std::map<K, V, C>& b) {
  return ordered_compare(a, b);
}

template <typename K, typename V, typename H, typename E>
inline int unified_compare(const std::unordered_map<K, V, H, E>& a,
                           const std::unordered_map<K, V, H, E>& b) {
  return unordered_compare(a, b);
}

template <class T> inline int ordered_compare(const T& A, const T& B) {
  // Can't be equal if # of entries differ:
  if (A.size() != B.size())
    return A.size() < B.size() ? -1 : 1;

  // Loop over elements in "a" and "b":
  auto a = A.begin();
  auto b = B.begin();
  for (; a != A.end(); ++a, ++b) {
    auto t = unified_compare(*a, *b);
    if (t != 0)
      return t; // values not equal
  }
  return 0;
}

template <class T> inline int ordered_compare(const T& A, const T& B);

template <class M, typename C = std::less<typename M::key_type>>
inline int unordered_compare(const M& a, const M& b) {
  // Can't be equal if # of entries differ:
  if (a.size() != b.size())
    return a.size() < b.size() ? -1 : 1;

  std::vector<typename M::key_type> va = sorted_keys<M, C>(a);
  std::vector<typename M::key_type> vb = sorted_keys<M, C>(b);

  for (unsigned int i = 0; i < va.size() && i < vb.size(); i++) {
    bool s = C()(va[i], vb[i]);
    if (s)
      return -1;
    s = C()(vb[i], va[i]);
    if (s)
      return 1;

    int t = unified_compare(a.find(va[i])->second, b.find(vb[i])->second);
    if (t != 0)
      return t;
  }
  return 0;
}

} // namespace whitemech::lydia