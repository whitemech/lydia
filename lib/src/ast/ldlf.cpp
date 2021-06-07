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

ldlf_ptr AstManager::makeLdlfTrue() { return ldlf_true_; }
ldlf_ptr AstManager::makeLdlfFalse() { return ldlf_false_; }
ldlf_ptr AstManager::makeLdlfBool(bool value) {
  return value ? ldlf_true_ : ldlf_false_;
}
ldlf_ptr AstManager::makeLdlfAnd(const set_formulas& args) {
  ldlf_ptr (AstManager::*fun)(bool) = &AstManager::makeLdlfBool;
  auto tmp = and_or<const LDLfFormula, LDLfAnd, LDLfTrue, LDLfFalse, LDLfNot,
                    LDLfAnd, LDLfOr>(*this, args, false, fun);
  auto result = insert_if_not_available_(tmp);
  return result;
}
ldlf_ptr AstManager::makeLdlfOr(const set_formulas& args) {
  ldlf_ptr (AstManager::*fun)(bool) = &AstManager::makeLdlfBool;
  auto tmp = and_or<const LDLfFormula, LDLfOr, LDLfTrue, LDLfFalse, LDLfNot,
                    LDLfAnd, LDLfOr>(*this, args, true, fun);
  auto result = insert_if_not_available_(tmp);
  return result;
}
ldlf_ptr AstManager::makeLdlfNot(const ldlf_ptr& arg) {
  auto tmp = std::make_shared<const LDLfNot>(*this, arg);
  auto result = insert_if_not_available_(tmp);
  return result;
}

ldlf_ptr AstManager::makeLdlfBox(const regex_ptr& arg_r,
                                 const ldlf_ptr& arg_f) {
  auto tmp = std::make_shared<const LDLfBox>(*this, arg_r, arg_f);
  auto result = insert_if_not_available_(tmp);
  return result;
}
ldlf_ptr AstManager::makeLdlfDiamond(const regex_ptr& arg_r,
                                     const ldlf_ptr& arg_f) {
  auto tmp = std::make_shared<const LDLfDiamond>(*this, arg_r, arg_f);
  auto result = insert_if_not_available_(tmp);
  return result;
}

ldlf_ptr AstManager::makeLdlfF(const ldlf_ptr& arg) {
  auto tmp = std::make_shared<const LDLfF>(*this, arg);
  auto result = insert_if_not_available_(tmp);
  return result;
}
ldlf_ptr AstManager::makeLdlfT(const ldlf_ptr& arg) {
  auto tmp = std::make_shared<const LDLfT>(*this, arg);
  auto result = insert_if_not_available_(tmp);
  return result;
}

ldlf_ptr AstManager::makeLdlfQ(const ldlf_ptr& arg) {
  auto tmp = std::make_shared<const LDLfQ>(*this, arg);
  auto result = insert_if_not_available_(tmp);
  return result;
}

ldlf_ptr AstManager::makeLdlfEnd() {
  return makeLdlfBox(makePropRegex(makeTrue()), makeLdlfFalse());
}

} // namespace whitemech::lydia