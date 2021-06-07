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
#include <lydia/logic/ltlf/base.hpp>

namespace whitemech::lydia {

ltlf_ptr AstManager::makeLtlfTrue() { return ltlf_true_; }
ltlf_ptr AstManager::makeLtlfFalse() { return ltlf_false_; }
ltlf_ptr AstManager::makeLtlfBool(bool value) {
  return value ? ltlf_true_ : ltlf_false_;
}
ltlf_ptr AstManager::makeLtlfAtom(const std::string& name) {
  auto tmp = std::make_shared<const LTLfAtom>(*this, name);
  auto result = insert_if_not_available_(tmp);
  return result;
}

ltlf_ptr AstManager::makeLtlfAnd(const set_ltlf_formulas& args) {
  ltlf_ptr (AstManager::*fun)(bool) = &AstManager::makeLtlfBool;
  auto tmp = and_or<const LTLfFormula, LTLfAnd, LTLfTrue, LTLfFalse, LTLfNot,
                    LTLfAnd, LTLfOr>(*this, args, false, fun);
  auto result = insert_if_not_available_(tmp);
  return result;
}
ltlf_ptr AstManager::makeLtlfOr(const set_ltlf_formulas& args) {
  ltlf_ptr (AstManager::*fun)(bool) = &AstManager::makeLtlfBool;
  auto tmp = and_or<const LTLfFormula, LTLfOr, LTLfTrue, LTLfFalse, LTLfNot,
                    LTLfAnd, LTLfOr>(*this, args, true, fun);
  auto result = insert_if_not_available_(tmp);
  return result;
}
ltlf_ptr AstManager::makeLtlfNot(const ltlf_ptr& arg) {
  auto tmp = std::make_shared<const LTLfNot>(*this, arg);
  auto result = insert_if_not_available_(tmp);
  return result;
}
ltlf_ptr AstManager::makeLtlfNext(const ltlf_ptr& arg) {
  auto tmp = std::make_shared<const LTLfNext>(*this, arg);
  auto result = insert_if_not_available_(tmp);
  return result;
}
ltlf_ptr AstManager::makeLtlfWeakNext(const ltlf_ptr& arg) {
  auto tmp = std::make_shared<const LTLfWeakNext>(*this, arg);
  auto result = insert_if_not_available_(tmp);
  return result;
}
ltlf_ptr AstManager::makeLtlfUntil(const ltlf_ptr& arg_1,
                                   const ltlf_ptr& arg_2) {
  auto tmp = std::make_shared<const LTLfUntil>(*this, arg_1, arg_2);
  auto result = insert_if_not_available_(tmp);
  return result;
}
ltlf_ptr AstManager::makeLtlfRelease(const ltlf_ptr& arg_1,
                                     const ltlf_ptr& arg_2) {
  auto tmp = std::make_shared<const LTLfRelease>(*this, arg_1, arg_2);
  auto result = insert_if_not_available_(tmp);
  return result;
}
ltlf_ptr AstManager::makeLtlfEventually(const ltlf_ptr& arg) {
  auto tmp = std::make_shared<const LTLfEventually>(*this, arg);
  auto result = insert_if_not_available_(tmp);
  return result;
}
ltlf_ptr AstManager::makeLtlfAlways(const ltlf_ptr& arg) {
  auto tmp = std::make_shared<const LTLfAlways>(*this, arg);
  auto result = insert_if_not_available_(tmp);
  return result;
}

} // namespace whitemech::lydia