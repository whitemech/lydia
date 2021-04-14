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

#include <lydia/ast/base.hpp>
#include <lydia/logic/ldlf/base.hpp>
#include <lydia/logic/ltlf/base.hpp>
#include <lydia/logic/pl/base.hpp>

namespace whitemech::lydia {

void AstManager::init() {
  prop_true_ = std::make_shared<const PropositionalTrue>(*this);
  prop_false_ = std::make_shared<const PropositionalFalse>(*this);
  ltlf_true_ = std::make_shared<const LTLfTrue>(*this);
  ltlf_false_ = std::make_shared<const LTLfFalse>(*this);
  ldlf_true_ = std::make_shared<const LDLfTrue>(*this);
  ldlf_false_ = std::make_shared<const LDLfFalse>(*this);
  this->table.insert(prop_true_);
  this->table.insert(prop_false_);
  this->table.insert(ltlf_true_);
  this->table.insert(ltlf_false_);
  this->table.insert(ldlf_true_);
  this->table.insert(ldlf_false_);
}

} // namespace whitemech::lydia