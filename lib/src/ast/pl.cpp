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

template <typename caller>
prop_ptr and_or(AstManager &context, const set_prop_formulas &s,
                const bool &op_x_notx) {
  set_prop_formulas args;
  for (auto &a : s) {
    // handle the case when a subformula is true
    if (is_a<PropositionalTrue>(*a)) {
      if (op_x_notx)
        return a;
      else
        continue;
    }
    // handle the case when a subformula is false
    else if (is_a<PropositionalFalse>(*a)) {
      if (!op_x_notx)
        return a;
      else
        continue;
    }
    // handle the case when a subformula is of the same type of the caller
    else if (is_a<caller>(*a)) {
      const auto &to_insert = dynamic_cast<const caller &>(*a);
      const auto &container = to_insert.get_container();
      args.insert(container.begin(), container.end());
      continue;
    } else {
      args.insert(a);
    }
  }
  for (auto &a : args) {
    if (args.find(a->logical_not()) != args.end())
      return context.makeBool(op_x_notx);
  }
  if (args.size() == 1)
    return *(args.begin());
  else if (args.empty())
    return context.makeBool(not op_x_notx);
  return std::make_shared<caller>(context, args);
}

prop_ptr AstManager::makeTrue() { return prop_true_; }

prop_ptr AstManager::makeFalse() { return prop_false_; }

symbol_ptr AstManager::makeSymbol(const std::string &name) {
  auto tmp = std::make_shared<const Symbol>(name);
  auto result = insert_if_not_available_(tmp);
  return result;
}

atom_ptr AstManager::makePropAtom(const std::string &name) {
  auto tmp = std::make_shared<const PropositionalAtom>(*this, name);
  auto result = insert_if_not_available_(tmp);
  return result;
}

atom_ptr AstManager::makePropAtom(const basic_ptr &ptr) {
  auto tmp = std::make_shared<const PropositionalAtom>(*this, ptr);
  auto result = insert_if_not_available_(tmp);
  return result;
}

prop_ptr AstManager::makePropAnd(const set_prop_formulas &args) {
  auto tmp = and_or<PropositionalAnd>(*this, args, false);
  auto result = insert_if_not_available_(tmp);
  return result;
}

prop_ptr AstManager::makePropOr(const set_prop_formulas &args) {
  auto tmp = and_or<PropositionalOr>(*this, args, true);
  auto result = insert_if_not_available_(tmp);
  return result;
}

prop_ptr AstManager::makePropNot(const prop_ptr &arg) {
  auto tmp = std::make_shared<const PropositionalNot>(*this, arg);
  auto result = insert_if_not_available_(tmp);
  return result;
}

prop_ptr AstManager::makeBool(bool value) {
  return value ? prop_true_ : prop_false_;
}

} // namespace whitemech::lydia