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
#include <lydia/logic/pl/base.hpp>

namespace whitemech::lydia {

prop_ptr AstManager::makeTrue() { return prop_true_; }

prop_ptr AstManager::makeFalse() { return prop_false_; }

symbol_ptr AstManager::makeSymbol(const std::string& name) {
  auto tmp = std::make_shared<const Symbol>(name);
  auto result = insert_if_not_available_(tmp);
  return result;
}

atom_ptr AstManager::makePropAtom(const std::string& name) {
  auto tmp = std::make_shared<const PropositionalAtom>(*this, name);
  auto result = insert_if_not_available_(tmp);
  return result;
}

atom_ptr AstManager::makePropAtom(const basic_ptr& ptr) {
  auto tmp = std::make_shared<const PropositionalAtom>(*this, ptr);
  auto result = insert_if_not_available_(tmp);
  return result;
}

prop_ptr AstManager::makePropAnd(const set_prop_formulas& args) {
  prop_ptr (AstManager::*fun)(bool) = &AstManager::makeBool;
  auto tmp = and_or<const PropositionalFormula, PropositionalAnd,
                    PropositionalTrue, PropositionalFalse, PropositionalNot,
                    PropositionalAnd, PropositionalOr>(*this, args, false, fun);
  auto result = insert_if_not_available_(tmp);
  return result;
}

prop_ptr AstManager::makePropOr(const set_prop_formulas& args) {
  prop_ptr (AstManager::*fun)(bool) = &AstManager::makeBool;
  auto tmp = and_or<const PropositionalFormula, PropositionalOr,
                    PropositionalTrue, PropositionalFalse, PropositionalNot,
                    PropositionalAnd, PropositionalOr>(*this, args, true, fun);
  auto result = insert_if_not_available_(tmp);
  return result;
}

prop_ptr AstManager::makePropNot(const prop_ptr& arg) {
  auto tmp = std::make_shared<const PropositionalNot>(*this, arg);
  auto result = insert_if_not_available_(tmp);
  return result;
}

prop_ptr AstManager::makeBool(bool value) {
  return value ? prop_true_ : prop_false_;
}

} // namespace whitemech::lydia