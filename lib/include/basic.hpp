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

#include <functional>

#include "types.hpp"

namespace whitemech {
namespace lydia {

enum TypeID { t_Symbol, t_LDLfBooleanAtom, t_LDLfAnd, t_LDLfOr, t_LDLfNot };

class Visitor;
class Symbol;

class Basic {
private:
  // The hash_ is defined as mutable, because its value is initialized to 0
  // in the constructor and then it can be changed in Basic::hash() to the
  // current hash (which is always the same for the given instance). The
  // state of the instance does not change, so we define hash_ as mutable
  mutable hash_t hash_; // This holds the hash value
public:
  TypeID type_code_;

  inline TypeID get_type_code() const { return type_code_; };

  // Destructor must be explicitly defined as virtual here to avoid problems
  // with undefined behavior while deallocating derived classes.
  Basic() : hash_{0} {}

  //! Delete the copy constructor and assignment
  Basic(const Basic &) = delete;

  //! Assignment operator in continuation with above
  Basic &operator=(const Basic &) = delete;

  //! Delete the move constructor and assignment
  Basic(Basic &&) = delete;

  //! Assignment operator in continuation with above
  Basic &operator=(Basic &&) = delete;

  // Equality operator
  virtual bool is_equal(const Basic &o) const = 0;

  /*!
  Calculates the hash of the given Lydia class.
  Use Basic.hash() which gives a cached version of the hash.
  \return 64-bit integer value for the hash
  */
  virtual hash_t __hash__() const = 0;

  /*! Returns the hash of the Basic class:
      This method caches the value.

      Use `std::hash` to get the hash. Example:

           const Symbol x = symbol("x");
           std::hash<Basic> hash_fn;
           std::cout << hash_fn(*x);

      \return 64-bit integer value for the hash
  */
  hash_t hash() const;

  /*! Returns -1, 0, 1 for `this < o, this == o, this > o`. This method is
   used      when you want to sort things like `x+y+z` into canonical order.
   This function assumes that `o` is the same type as `this`. Use ` __cmp__`
   if you want general comparison.
   */
  virtual int compare(const Basic &o) const = 0;
  int __cmp__(const Basic &o) const;

  std::string str() const;

  //        virtual void accept(Visitor &v) const;
};

// TODO decide what to do with this:
//    std::ostream &operator<<(std::ostream &strm, const Basic &b) {
//        return strm << b.str();
//    }

template <class T> void hash_combine(hash_t &seed, const T &v);

// Inline members and functions
inline hash_t Basic::hash() const {
  if (hash_ == 0)
    hash_ = __hash__();
  return hash_;
}

//! \return true if  `a` equal `b`
inline bool eq(const Basic &a, const Basic &b) {
  if (&a == &b) {
    return true;
  }
  return a.is_equal(b);
}
//! \return true if  `a` not equal `b`
inline bool neq(const Basic &a, const Basic &b) { return not(a.is_equal(b)); }

//! Templatised version to check is_a type
template <class T> inline bool is_a(const Basic &b) {
  return T::type_code_id == b.get_type_code();
}

template <class T> inline bool is_a_sub(const Basic &b) {
  return dynamic_cast<const T *>(&b) != nullptr;
}

inline bool is_same_type(const Basic &a, const Basic &b) {
  return a.get_type_code() == b.get_type_code();
}

//! Templatised version to combine hash
template <typename T>
inline void hash_combine_impl(
    hash_t &seed, const T &v,
    typename std::enable_if<std::is_base_of<Basic, T>::value>::type * =
        nullptr) {
  hash_combine(seed, v.hash());
}

template <typename T>
inline void hash_combine_impl(
    hash_t &seed, const T &v,
    typename std::enable_if<std::is_integral<T>::value>::type * = nullptr) {
  seed ^= hash_t(v) + hash_t(0x9e3779b9) + (seed << 6) + (seed >> 2);
}

inline void hash_combine_impl(hash_t &seed, const std::string &s) {
  for (const char &c : s) {
    hash_combine<hash_t>(seed, static_cast<hash_t>(c));
  }
}

inline void hash_combine_impl(hash_t &seed, const double &s) {
  union {
    hash_t h;
    double d;
  } u{};
  u.h = 0u;
  u.d = s;
  hash_combine(seed, u.h);
}

template <class T> inline void hash_combine(hash_t &seed, const T &v) {
  hash_combine_impl(seed, v);
}

} // namespace lydia
} // namespace whitemech

// std namespace functions
namespace std {
//! Specialise std::hash for Basic. We just call Basic.__hash__()
template <> struct hash<whitemech::lydia::Basic> {
  std::size_t operator()(const whitemech::lydia::Basic &b) const {
    return b.hash();
  }
};
} // namespace std