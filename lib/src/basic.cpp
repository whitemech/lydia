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

#include <lydia/basic.hpp>
#include <lydia/utils/print.hpp>

namespace whitemech::lydia {

int Basic::compare(const Basic& o) const {
  auto a = this->get_type_code();
  auto b = o.get_type_code();
  if (a == b) {
    return this->compare_(o);
  } else {
    // We return the order given by the numerical value of the TypeID enum
    // type.
    // The types don't need to be ordered in any given way, they just need
    // to be ordered.
    return a < b ? -1 : 1;
  }
}

std::string Basic::str() const { return to_string(*this); }

} // namespace whitemech::lydia