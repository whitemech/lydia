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

#include "logic.hpp"
#include "utils/compare.hpp"
#include <cassert>
#include <utility>

namespace whitemech {
namespace lydia {

LDLfBooleanAtom::LDLfBooleanAtom(bool b) : b_{b} {
  this->type_code_ = type_code_id;
}

hash_t LDLfBooleanAtom::__hash__() const {
  hash_t seed = TypeID::t_LDLfBooleanAtom;
  if (b_)
    ++seed;
  return seed;
}

bool LDLfBooleanAtom::get_value() const { return b_; }

vec_basic LDLfBooleanAtom::get_args() const { return {}; }

bool LDLfBooleanAtom::is_equal(const Basic &o) const {
  return is_a<LDLfBooleanAtom>(o) and
         get_value() == dynamic_cast<const LDLfBooleanAtom &>(o).get_value();
}

int LDLfBooleanAtom::compare(const Basic &o) const {
  assert(is_a<LDLfBooleanAtom>(o));
  bool ob = dynamic_cast<const LDLfBooleanAtom &>(o).get_value();
  if (get_value()) {
    return (ob) ? 0 : 1;
  } else {
    return (ob) ? -1 : 0;
  }
}

const LDLfFormula &LDLfBooleanAtom::logical_not() const {
  return *boolean(not this->get_value());
}

bool LDLfBooleanAtom::operator==(const Basic &o) const { return is_equal(o); }
bool LDLfBooleanAtom::operator!=(const Basic &o) const { return !is_equal(o); }

And::And(set_formulas s) : container_{std::move(s)} {
  this->type_code_ = type_code_id;
  assert(is_canonical(s));
}

hash_t And::__hash__() const {
  hash_t seed = TypeID::t_LDLfAnd;
  for (const auto &a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

bool And::is_canonical(const set_formulas &container_) {
  // TODO do some checks on the arguments
  return true;
}

vec_basic And::get_args() const {
  vec_basic v(container_.begin(), container_.end());
  return v;
}

bool And::is_equal(const Basic &o) const {
  assert(false);
  //    return is_a<And>(o)
  //           and unified_eq(container_,
  //                          dynamic_cast<const And &>(o).get_container());
}

int And::compare(const Basic &o) const {
  assert(false);
  //    assert(is_a<And>(o));
  //    return unified_compare(container_,
  //                           dynamic_cast<const And &>(o).get_container());
}

const set_formulas &And::get_container() const { return container_; }

// const LDLfFormula And::logical_not() const
//{
//    auto container = this->get_container();
//    set_boolean cont;
//    for (auto &a : container) {
//        cont.insert(logical_not(a));
//    }
//    return make_rcp<const Or>(cont);
//}

Or::Or(set_formulas s) : container_{std::move(s)} {
  this->type_code_ = type_code_id;
}

hash_t Or::__hash__() const {
  hash_t seed = TypeID::t_LDLfOr;
  for (const auto &a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

vec_basic Or::get_args() const {
  vec_basic v(container_.begin(), container_.end());
  return v;
}

bool Or::is_equal(const Basic &o) const {
  assert(false);
  //    return is_a<Or>(o)
  //           and unified_eq(container_, dynamic_cast<const Or
  //           &>(o).get_container());
}

int Or::compare(const Basic &o) const {
  assert(false);
  //    assert(is_a<Or>(o));
  //    return unified_compare(container_,
  //                           dynamic_cast<const Or &>(o).get_container());
}

bool Or::is_canonical(const set_formulas &container_) {
  // TODO do some checks on the arguments
  return true;
}

const set_formulas &Or::get_container() const { return container_; }

// const Or::logical_not() const
//{
//    auto container = this->get_container();
//    set_boolean cont;
//    for (auto &a : container) {
//        cont.insert(logical_not(a));
//    }
//    return const And(cont);
//}

Not::Not(const std::shared_ptr<LDLfFormula> &in) : arg_{std::move(in)} {
  this->type_code_ = type_code_id;
  assert(is_canonical(*in));
}

hash_t Not::__hash__() const {
  hash_t seed = TypeID::t_LDLfNot;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

vec_basic Not::get_args() const {
  vec_basic v;
  v.push_back(arg_);
  return v;
}

bool Not::is_equal(const Basic &o) const {
  assert(false);
  //    return is_a<Not>(o) and eq(arg_, dynamic_cast<const Not
  //    &>(o).get_arg());
}

int Not::compare(const Basic &o) const {
  assert(is_a<Not>(o));
  return arg_->__cmp__(dynamic_cast<const Not &>(o).get_arg());
}

bool Not::is_canonical(const LDLfFormula &in) {
  // TODO do some checks on the argument
  return true;
}

const LDLfFormula &Not::get_arg() const { return *arg_; }

// const LDLfFormula Not::logical_not() const
//{
//    return this->get_arg();
//}

} // namespace lydia
} // namespace whitemech
