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

namespace whitemech::lydia {

regex_ptr AstManager::makePropRegex(const prop_ptr& ptr) {
  auto tmp = std::make_shared<const PropositionalRegExp>(*this, ptr);
  auto result = insert_if_not_available_(tmp);
  return result;
}

regex_ptr AstManager::makeSeqRegex(const vec_regex& ptr) {
  auto tmp = flatten_bin_op_vec<const RegExp, SequenceRegExp>(*this, ptr);
  auto result = insert_if_not_available_(tmp);
  return result;
}

regex_ptr AstManager::makeUnionRegex(const set_regex& ptr) {
  auto tmp = flatten_bin_op_set<const RegExp, UnionRegExp>(*this, ptr);
  auto result = insert_if_not_available_(tmp);
  return result;
}

regex_ptr AstManager::makeStarRegex(const regex_ptr& ptr) {
  auto tmp = std::make_shared<const StarRegExp>(*this, ptr);
  auto result = insert_if_not_available_(tmp);
  return result;
}

regex_ptr AstManager::makeTestRegex(const ldlf_ptr& ptr) {
  auto tmp = std::make_shared<const TestRegExp>(*this, ptr);
  auto result = insert_if_not_available_(tmp);
  return result;
}

} // namespace whitemech::lydia