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

#include "cuddObj.hh"
#include <cstdint>
#include <exception>
#include <memory>
#include <set>
#include <vector>

namespace whitemech {
namespace lydia {

class Basic;
class Symbol;
class LDLfFormula;

struct SharedComparator {
  template <typename T>
  bool operator()(const std::shared_ptr<T> &lhs,
                  const std::shared_ptr<T> &rhs) const {
    return (*lhs) < (*rhs);
  }
};

typedef uint64_t hash_t;
typedef std::vector<std::shared_ptr<const Basic>> vec_basic;
typedef std::set<std::shared_ptr<const Basic>, SharedComparator> set_basic;
typedef std::vector<std::shared_ptr<const LDLfFormula>> vec_formulas;
typedef std::set<std::shared_ptr<const LDLfFormula>, SharedComparator>
    set_formulas;
typedef std::vector<int> item;
typedef std::vector<int> interpretation;
typedef std::vector<interpretation> trace;
typedef std::vector<CUDD::BDD> vbdd;
typedef std::vector<CUDD::BDD *> vbdd_ptr;

class not_implemented_error : public std::logic_error {
public:
  not_implemented_error() : std::logic_error("Function not yet implemented"){};
};

} // namespace lydia
}; // namespace whitemech
