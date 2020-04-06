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

#include "basic.hpp"
#include "symbol.hpp"

namespace whitemech {
namespace lydia {

class LDLfFormula : public Basic {
  //    const LDLfFormula logical_not() const;
};

// Temporal True and False
class LDLfBooleanAtom : public LDLfFormula {

private:
  bool b_;

public:
  const static TypeID type_code_id = TypeID::t_LDLfBooleanAtom;
  explicit LDLfBooleanAtom(bool b);
  //! \return the hash
  hash_t __hash__() const override;
  bool get_value() const;
  virtual vec_basic get_args() const;
  bool is_equal(const Basic &o) const override;
  bool operator==(const Basic &o) const;
  bool operator!=(const Basic &o) const;
  int compare(const Basic &o) const override;
  const LDLfFormula &logical_not() const;
};

extern const LDLfBooleanAtom boolTrue;
extern const LDLfBooleanAtom boolFalse;

inline const LDLfBooleanAtom &boolean(bool b) {
  return const_cast<LDLfBooleanAtom &>(b ? boolTrue : boolFalse);
}

} // namespace lydia
} // namespace whitemech
