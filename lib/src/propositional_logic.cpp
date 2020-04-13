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
#include <cassert>
#include <propositional_logic.hpp>
#include <utils/compare.hpp>

namespace whitemech {
namespace lydia {

hash_t PropositionalTrue::__hash__() const {
  hash_t seed = type_code_id;
  return seed;
}

int PropositionalTrue::compare(const Basic &rhs) const {
  return is_a<PropositionalTrue>(rhs);
}

bool PropositionalTrue::is_equal(const Basic &rhs) const {
  return is_a<PropositionalTrue>(rhs);
};

hash_t PropositionalFalse::__hash__() const {
  hash_t seed = type_code_id;
  return seed;
}

int PropositionalFalse::compare(const Basic &rhs) const {
  return is_a<PropositionalFalse>(rhs);
}

bool PropositionalFalse::is_equal(const Basic &rhs) const {
  return is_a<PropositionalFalse>(rhs);
}

PropositionalAtom::PropositionalAtom(const Symbol &s) : symbol{s.get_name()} {}

PropositionalAtom::PropositionalAtom(const std::string &name)
    : symbol(Symbol(name)) {}

hash_t PropositionalAtom::__hash__() const { return this->symbol.__hash__(); }

int PropositionalAtom::compare(const Basic &rhs) const {
  return this->symbol.compare(rhs);
}

bool PropositionalAtom::is_equal(const Basic &rhs) const {
  return this->symbol.is_equal(rhs);
}

PropositionalAnd::PropositionalAnd(const set_prop_formulas &s) : container_{s} {
  this->type_code_ = type_code_id;
  assert(is_canonical(s));
}

hash_t PropositionalAnd::__hash__() const {
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

PropositionalOr::PropositionalOr(const set_prop_formulas &s) : container_{s} {
  this->type_code_ = type_code_id;
}

hash_t PropositionalOr::__hash__() const {
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

PropositionalNot::PropositionalNot(
    const std::shared_ptr<const PropositionalFormula> &in)
    : arg_{in} {
  this->type_code_ = type_code_id;
  assert(is_canonical(*in));
}

hash_t PropositionalNot::__hash__() const {
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
  return arg_->__cmp__(*dynamic_cast<const PropositionalNot &>(o).get_arg());
}

std::shared_ptr<const PropositionalFormula> PropositionalNot::get_arg() const {
  return arg_;
}

} // namespace lydia
} // namespace whitemech
