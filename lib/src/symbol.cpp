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

#include <lydia/logic/symbol.hpp>

#include <cassert>
#include <utility>

namespace whitemech::lydia {

Symbol::Symbol(std::string name) : name_{std::move(name)} {
  this->type_code_ = type_code_id;
}

bool Symbol::is_equal(const Basic& o) const {
  if (is_a<Symbol>(o))
    // TODO: check symengine_casts.h
    return name_ == dynamic_cast<const Symbol&>(o).name_;
  return false;
}

int Symbol::compare_(const Basic& o) const {
  assert(is_a<Symbol>(o));
  const auto& s = dynamic_cast<const Symbol&>(o);
  if (name_ == s.name_)
    return 0;
  return name_ < s.name_ ? -1 : 1;
}

hash_t Symbol::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine(seed, name_);
  return seed;
}

} // namespace whitemech::lydia
