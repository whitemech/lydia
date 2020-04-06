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

#include "logic.hpp"
#include <cassert>
#include <jmorecfg.h>

namespace whitemech {
namespace lydia {

LDLfBooleanAtom::LDLfBooleanAtom(bool b) : b_{b} {
  this->type_code_ = type_code_id;
}

hash_t LDLfBooleanAtom::__hash__() const {
  hash_t seed = TypeID::t_LDLfBooleanAtom;
  if (b_)
    ++seed;
  return seed;
}

bool LDLfBooleanAtom::get_value() const { return b_; }

vec_basic LDLfBooleanAtom::get_args() const { return {}; }

bool LDLfBooleanAtom::is_equal(const Basic &o) const {
  return is_a<LDLfBooleanAtom>(o) and
         get_value() == dynamic_cast<const LDLfBooleanAtom &>(o).get_value();
}

int LDLfBooleanAtom::compare(const Basic &o) const {
  assert(is_a<LDLfBooleanAtom>(o));
  bool ob = dynamic_cast<const LDLfBooleanAtom &>(o).get_value();
  if (get_value()) {
    return (ob) ? 0 : 1;
  } else {
    return (ob) ? -1 : 0;
  }
}

const LDLfFormula &LDLfBooleanAtom::logical_not() const {
  return boolean(not this->get_value());
}

const LDLfBooleanAtom boolTrue = LDLfBooleanAtom(true);
const LDLfBooleanAtom boolFalse = LDLfBooleanAtom(false);

bool LDLfBooleanAtom::operator==(const Basic &o) const { return is_equal(o); }
bool LDLfBooleanAtom::operator!=(const Basic &o) const { return !is_equal(o); }

} // namespace lydia
} // namespace whitemech
