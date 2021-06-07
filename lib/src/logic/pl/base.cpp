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
#include <cassert>
#include <lydia/logic/atom_visitor.hpp>
#include <lydia/logic/pl/base.hpp>
#include <lydia/utils/compare.hpp>
#include <lydia/utils/misc.hpp>
#include <utility>

namespace whitemech::lydia {

hash_t PropositionalTrue::compute_hash_() const {
  hash_t seed = type_code_id;
  return seed;
}

int PropositionalTrue::compare_(const Basic& rhs) const {
  assert(is_a<PropositionalTrue>(rhs));
  return 0;
}

bool PropositionalTrue::is_equal(const Basic& rhs) const {
  return is_a<PropositionalTrue>(rhs);
};

prop_ptr PropositionalTrue::logical_not() const {
  return this->m_ctx->makeFalse();
}

hash_t PropositionalFalse::compute_hash_() const {
  hash_t seed = type_code_id;
  return seed;
}

int PropositionalFalse::compare_(const Basic& rhs) const {
  assert(is_a<PropositionalFalse>(rhs));
  return 0;
}

bool PropositionalFalse::is_equal(const Basic& rhs) const {
  return is_a<PropositionalFalse>(rhs);
}

prop_ptr PropositionalFalse::logical_not() const {
  return this->m_ctx->makeTrue();
}

PropositionalAtom::PropositionalAtom(AstManager& c,
                                     std::shared_ptr<const Basic> p)
    : PropositionalFormula(c), symbol{std::move(p)} {
  this->type_code_ = type_code_id;
}

PropositionalAtom::PropositionalAtom(AstManager& c, const std::string& name)
    : PropositionalFormula(c), symbol(c.makeSymbol(name)) {
  this->type_code_ = type_code_id;
}

hash_t PropositionalAtom::compute_hash_() const {
  return this->symbol->compute_hash_();
}

int PropositionalAtom::compare_(const Basic& rhs) const {
  assert(is_a<PropositionalAtom>(rhs));
  return this->symbol->compare(
      *dynamic_cast<const PropositionalAtom&>(rhs).symbol);
}

bool PropositionalAtom::is_equal(const Basic& rhs) const {
  return is_a<PropositionalAtom>(rhs) and
         this->symbol->is_equal(
             *dynamic_cast<const PropositionalAtom&>(rhs).symbol);
}

prop_ptr PropositionalAtom::logical_not() const {
  prop_ptr ptr = m_ctx->makePropAtom(this->symbol->str());
  return m_ctx->makePropNot(ptr);
}

PropositionalAnd::PropositionalAnd(AstManager& c, const set_prop_formulas& s)
    : PropositionalFormula(c), container_{s} {
  this->type_code_ = type_code_id;
  assert(is_canonical(s));
}

hash_t PropositionalAnd::compute_hash_() const {
  hash_t seed = TypeID::t_PropositionalAnd;
  for (const auto& a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

vec_prop_formulas PropositionalAnd::get_args() const {
  vec_prop_formulas v(container_.begin(), container_.end());
  return v;
}

bool PropositionalAnd::is_equal(const Basic& o) const {
  return is_a<PropositionalAnd>(o) and
         unified_eq(container_,
                    dynamic_cast<const PropositionalAnd&>(o).get_container());
}

int PropositionalAnd::compare_(const Basic& o) const {
  assert(is_a<PropositionalAnd>(o));
  return unified_compare(
      container_, dynamic_cast<const PropositionalAnd&>(o).get_container());
}

set_prop_formulas PropositionalAnd::get_container() const { return container_; }

prop_ptr PropositionalAnd::logical_not() const {
  auto container = this->get_container();
  set_prop_formulas cont;
  for (auto& a : container) {
    cont.insert(a->logical_not());
  }
  return m_ctx->makePropOr(cont);
}

PropositionalOr::PropositionalOr(AstManager& c, const set_prop_formulas& s)
    : PropositionalFormula(c), container_{s} {
  this->type_code_ = type_code_id;
}

hash_t PropositionalOr::compute_hash_() const {
  hash_t seed = TypeID::t_PropositionalOr;
  for (const auto& a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

vec_prop_formulas PropositionalOr::get_args() const {
  vec_prop_formulas v(container_.begin(), container_.end());
  return v;
}

bool PropositionalOr::is_equal(const Basic& o) const {
  return is_a<PropositionalOr>(o) and
         unified_eq(container_,
                    dynamic_cast<const PropositionalOr&>(o).get_container());
}

int PropositionalOr::compare_(const Basic& o) const {
  assert(is_a<PropositionalOr>(o));
  return unified_compare(
      container_, dynamic_cast<const PropositionalOr&>(o).get_container());
}

bool PropositionalOr::is_canonical(const set_prop_formulas& container_) {
  // TODO do some checks on the arguments
  return true;
}

set_prop_formulas PropositionalOr::get_container() const { return container_; }

prop_ptr PropositionalOr::logical_not() const {
  auto container = this->get_container();
  set_prop_formulas cont;
  for (auto& a : container) {
    cont.insert(a->logical_not());
  }
  return m_ctx->makePropAnd(cont);
}

PropositionalNot::PropositionalNot(
    AstManager& c, const std::shared_ptr<const PropositionalFormula>& in)
    : PropositionalFormula(c), arg_{in} {
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

bool PropositionalNot::is_equal(const Basic& o) const {
  return is_a<PropositionalNot>(o) and
         eq(*arg_, *dynamic_cast<const PropositionalNot&>(o).get_arg());
}

int PropositionalNot::compare_(const Basic& o) const {
  assert(is_a<PropositionalNot>(o));
  return arg_->compare(*dynamic_cast<const PropositionalNot&>(o).get_arg());
}

std::shared_ptr<const PropositionalFormula> PropositionalNot::get_arg() const {
  return arg_;
}

prop_ptr PropositionalNot::logical_not() const { return this->get_arg(); }

set_prop_formulas PropositionalNot::get_container() const {
  return set_prop_formulas{arg_};
}

} // namespace whitemech::lydia
