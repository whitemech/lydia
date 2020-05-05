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
#include "pl/logic.hpp"
#include "utils/compare.hpp"
#include "utils/misc.hpp"
#include <atom_visitor.hpp>
#include <cassert>
#include <cryptominisat5/cryptominisat.h>

namespace whitemech {
namespace lydia {

prop_ptr prop_true = std::make_shared<PropositionalTrue>();
prop_ptr prop_false = std::make_shared<PropositionalFalse>();

hash_t PropositionalTrue::compute_hash_() const {
  hash_t seed = type_code_id;
  return seed;
}

int PropositionalTrue::compare(const Basic &rhs) const {
  assert(is_a<PropositionalTrue>(rhs));
  return 0;
}

bool PropositionalTrue::is_equal(const Basic &rhs) const {
  return is_a<PropositionalTrue>(rhs);
};

prop_ptr PropositionalTrue::logical_not() const { return prop_false; }

hash_t PropositionalFalse::compute_hash_() const {
  hash_t seed = type_code_id;
  return seed;
}

int PropositionalFalse::compare(const Basic &rhs) const {
  assert(is_a<PropositionalFalse>(rhs));
  return 0;
}

bool PropositionalFalse::is_equal(const Basic &rhs) const {
  return is_a<PropositionalFalse>(rhs);
}

prop_ptr PropositionalFalse::logical_not() const { return prop_true; }

PropositionalAtom::PropositionalAtom(const Symbol &s)
    : symbol{std::make_shared<const Symbol>(s.get_name())} {
  this->type_code_ = type_code_id;
}

PropositionalAtom::PropositionalAtom(const std::shared_ptr<const Basic> &p)
    : symbol{std::shared_ptr<const Basic>(p)} {
  this->type_code_ = type_code_id;
}

PropositionalAtom::PropositionalAtom(const std::string &name)
    : symbol(std::make_shared<Symbol>(name)) {
  this->type_code_ = type_code_id;
}

hash_t PropositionalAtom::compute_hash_() const {
  return this->symbol->compute_hash_();
}

int PropositionalAtom::compare(const Basic &rhs) const {
  assert(is_a<PropositionalAtom>(rhs));
  return this->symbol->compare_(
      *dynamic_cast<const PropositionalAtom &>(rhs).symbol);
}

bool PropositionalAtom::is_equal(const Basic &rhs) const {
  return is_a<PropositionalAtom>(rhs) and
         this->symbol->is_equal(
             *dynamic_cast<const PropositionalAtom &>(rhs).symbol);
}

prop_ptr PropositionalAtom::logical_not() const {
  prop_ptr ptr = std::make_shared<const PropositionalAtom>(this->symbol);
  return std::make_shared<PropositionalNot>(ptr);
}

PropositionalAnd::PropositionalAnd(const set_prop_formulas &s) : container_{s} {
  this->type_code_ = type_code_id;
  assert(is_canonical(s));
}

hash_t PropositionalAnd::compute_hash_() const {
  hash_t seed = TypeID::t_PropositionalAnd;
  for (const auto &a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

vec_prop_formulas PropositionalAnd::get_args() const {
  vec_prop_formulas v(container_.begin(), container_.end());
  return v;
}

bool PropositionalAnd::is_equal(const Basic &o) const {
  return is_a<PropositionalAnd>(o) and
         unified_eq(container_,
                    dynamic_cast<const PropositionalAnd &>(o).get_container());
}

int PropositionalAnd::compare(const Basic &o) const {
  assert(is_a<PropositionalAnd>(o));
  return unified_compare(
      container_, dynamic_cast<const PropositionalAnd &>(o).get_container());
}

const set_prop_formulas &PropositionalAnd::get_container() const {
  return container_;
}

prop_ptr PropositionalAnd::logical_not() const {
  auto container = this->get_container();
  set_prop_formulas cont;
  for (auto &a : container) {
    cont.insert(a->logical_not());
  }
  return std::make_shared<PropositionalOr>(cont);
}

PropositionalOr::PropositionalOr(const set_prop_formulas &s) : container_{s} {
  this->type_code_ = type_code_id;
}

hash_t PropositionalOr::compute_hash_() const {
  hash_t seed = TypeID::t_PropositionalOr;
  for (const auto &a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

vec_prop_formulas PropositionalOr::get_args() const {
  vec_prop_formulas v(container_.begin(), container_.end());
  return v;
}

bool PropositionalOr::is_equal(const Basic &o) const {
  return is_a<PropositionalOr>(o) and
         unified_eq(container_,
                    dynamic_cast<const PropositionalOr &>(o).get_container());
}

int PropositionalOr::compare(const Basic &o) const {
  assert(is_a<PropositionalOr>(o));
  return unified_compare(
      container_, dynamic_cast<const PropositionalOr &>(o).get_container());
}

bool PropositionalOr::is_canonical(const set_prop_formulas &container_) {
  // TODO do some checks on the arguments
  return true;
}

const set_prop_formulas &PropositionalOr::get_container() const {
  return container_;
}

prop_ptr PropositionalOr::logical_not() const {
  auto container = this->get_container();
  set_prop_formulas cont;
  for (auto &a : container) {
    cont.insert(a->logical_not());
  }
  return std::make_shared<PropositionalAnd>(cont);
}

PropositionalNot::PropositionalNot(
    const std::shared_ptr<const PropositionalFormula> &in)
    : arg_{in} {
  this->type_code_ = type_code_id;
  assert(is_canonical(*in));
}

hash_t PropositionalNot::compute_hash_() const {
  hash_t seed = TypeID::t_PropositionalNot;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

vec_basic PropositionalNot::get_args() const {
  vec_basic v;
  v.push_back(arg_);
  return v;
}

bool PropositionalNot::is_equal(const Basic &o) const {
  return is_a<PropositionalNot>(o) and
         eq(*arg_, *dynamic_cast<const PropositionalNot &>(o).get_arg());
}

int PropositionalNot::compare(const Basic &o) const {
  assert(is_a<PropositionalNot>(o));
  return arg_->compare_(*dynamic_cast<const PropositionalNot &>(o).get_arg());
}

std::shared_ptr<const PropositionalFormula> PropositionalNot::get_arg() const {
  return arg_;
}

prop_ptr PropositionalNot::logical_not() const { return this->get_arg(); }

prop_ptr boolean_prop(bool b) { return b ? prop_true : prop_false; }

template <typename caller>
prop_ptr and_or(const set_prop_formulas &s, const bool &op_x_notx) {
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
    if (args.find(logical_not(a)) != args.end())
      return boolean_prop(op_x_notx);
  }
  if (args.size() == 1)
    return *(args.begin());
  else if (args.empty())
    return boolean_prop(not op_x_notx);
  return std::make_shared<caller>(s);
}

atom_ptr prop_atom(const Symbol &s) {
  return std::make_shared<const PropositionalAtom>(s);
}

atom_ptr prop_atom(const std::string &s) {
  return std::make_shared<const PropositionalAtom>(s);
}

prop_ptr logical_and(const set_prop_formulas &s) {
  return and_or<PropositionalAnd>(s, false);
}

prop_ptr logical_or(const set_prop_formulas &s) {
  return and_or<PropositionalOr>(s, true);
}

prop_ptr logical_not(const prop_ptr &f) { return f->logical_not(); }

} // namespace lydia
} // namespace whitemech
