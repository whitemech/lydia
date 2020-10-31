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

#include <lydia/ast/base.hpp>
#include <lydia/logic/pl/base.hpp>

namespace whitemech::lydia {

void AstManager::init() {
  prop_true_ = std::make_shared<const PropositionalTrue>(*this);
  prop_false_ = std::make_shared<const PropositionalFalse>(*this);
  this->table.insert(prop_true_);
  this->table.insert(prop_false_);
}

} // namespace whitemech::lydia