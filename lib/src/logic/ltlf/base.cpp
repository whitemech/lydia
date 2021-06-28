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
#include <lydia/logic/ltlf/base.hpp>
#include <lydia/utils/compare.hpp>
#include <stdexcept>
#include <utility>

namespace whitemech::lydia {

hash_t LTLfTrue::compute_hash_() const { return type_code_id; }

vec_ltlf_formulas LTLfTrue::get_args() const { return {}; }

bool LTLfTrue::is_equal(const Basic& o) const { return is_a<LTLfTrue>(o); }

int LTLfTrue::compare_(const Basic& o) const {
  assert(is_a<LTLfTrue>(o));
  return 0;
}

ltlf_ptr LTLfTrue::logical_not() const { return ctx().makeLtlfFalse(); }

///////////////////////////////////

hash_t LTLfFalse::compute_hash_() const { return type_code_id; }

vec_ltlf_formulas LTLfFalse::get_args() const { return {}; }

bool LTLfFalse::is_equal(const Basic& o) const { return is_a<LTLfFalse>(o); }

int LTLfFalse::compare_(const Basic& o) const {
  assert(is_a<LTLfFalse>(o));
  return 0;
}

ltlf_ptr LTLfFalse::logical_not() const { return ctx().makeLtlfTrue(); }

LTLfAtom::LTLfAtom(AstManager& c, const std::string& s)
    : LTLfFormula(c), symbol{c.makeSymbol(s)} {
  this->type_code_ = type_code_id;
}

LTLfAtom::LTLfAtom(AstManager& c, const symbol_ptr& p)
    : LTLfAtom(c, p->str()){};

hash_t LTLfAtom::compute_hash_() const { return this->symbol->compute_hash_(); }

int LTLfAtom::compare_(const Basic& rhs) const {
  assert(is_a<LTLfAtom>(rhs));
  return this->symbol->compare(*dynamic_cast<const LTLfAtom&>(rhs).symbol);
}

bool LTLfAtom::is_equal(const Basic& rhs) const {
  return is_a<LTLfAtom>(rhs) and
         this->symbol->is_equal(*dynamic_cast<const LTLfAtom&>(rhs).symbol);
}

ltlf_ptr LTLfAtom::logical_not() const {
  ltlf_ptr ptr = m_ctx->makeLtlfAtom(this->symbol->str());
  return m_ctx->makeLtlfNot(ptr);
}

LTLfAnd::LTLfAnd(AstManager& c, const set_ltlf_formulas& s)
    : LTLfFormula(c), container_{s} {
  this->type_code_ = type_code_id;
  if (!is_canonical(s)) {
    throw std::invalid_argument("LTLfAnd formula: arguments must be > 1");
  }
}

hash_t LTLfAnd::compute_hash_() const {
  hash_t seed = type_code_id;
  for (const auto& a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

bool LTLfAnd::is_canonical(const set_ltlf_formulas& container_) const {
  return container_.size() > 1;
}

vec_ltlf_formulas LTLfAnd::get_args() const {
  vec_ltlf_formulas v(container_.begin(), container_.end());
  return v;
}

bool LTLfAnd::is_equal(const Basic& o) const {
  return is_a<LTLfAnd>(o) and
         unified_eq(container_,
                    dynamic_cast<const LTLfAnd&>(o).get_container());
}

int LTLfAnd::compare_(const Basic& o) const {
  assert(is_a<LTLfAnd>(o));
  return unified_compare(container_,
                         dynamic_cast<const LTLfAnd&>(o).get_container());
}

const set_ltlf_formulas& LTLfAnd::get_container() const { return container_; }

ltlf_ptr LTLfAnd::logical_not() const {
  auto container = this->get_container();
  set_ltlf_formulas cont;
  for (auto& a : container) {
    cont.insert(a->logical_not());
  }
  return m_ctx->makeLtlfOr(cont);
}

LTLfOr::LTLfOr(AstManager& c, const set_ltlf_formulas& s)
    : LTLfFormula(c), container_{s} {
  this->type_code_ = type_code_id;
  if (!is_canonical(s)) {
    throw std::invalid_argument("LTLfOr formula: arguments must be > 1");
  }
}

hash_t LTLfOr::compute_hash_() const {
  hash_t seed = type_code_id;
  for (const auto& a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

vec_ltlf_formulas LTLfOr::get_args() const {
  vec_ltlf_formulas v(container_.begin(), container_.end());
  return v;
}

bool LTLfOr::is_equal(const Basic& o) const {
  return is_a<LTLfOr>(o) and
         unified_eq(container_, dynamic_cast<const LTLfOr&>(o).get_container());
}

int LTLfOr::compare_(const Basic& o) const {
  assert(is_a<LTLfOr>(o));
  return unified_compare(container_,
                         dynamic_cast<const LTLfOr&>(o).get_container());
}

bool LTLfOr::is_canonical(const set_ltlf_formulas& container_) const {
  return container_.size() > 1;
}

const set_ltlf_formulas& LTLfOr::get_container() const { return container_; }

ltlf_ptr LTLfOr::logical_not() const {
  auto container = this->get_container();
  set_ltlf_formulas cont;
  for (auto& a : container) {
    cont.insert(a->logical_not());
  }
  return m_ctx->makeLtlfAnd(cont);
}

LTLfNot::LTLfNot(AstManager& c, const ltlf_ptr& in) : LTLfFormula(c), arg_{in} {
  type_code_ = type_code_id;
}

hash_t LTLfNot::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

vec_basic LTLfNot::get_args() const {
  vec_basic v;
  v.push_back(arg_);
  return v;
}

bool LTLfNot::is_equal(const Basic& o) const {
  return is_a<LTLfNot>(o) and
         eq(*arg_, *dynamic_cast<const LTLfNot&>(o).get_arg());
}

int LTLfNot::compare_(const Basic& o) const {
  assert(is_a<LTLfNot>(o));
  return arg_->compare(*dynamic_cast<const LTLfNot&>(o).get_arg());
}

bool LTLfNot::is_canonical(const LTLfFormula& in) const { return true; }

ltlf_ptr LTLfNot::get_arg() const { return arg_; }

ltlf_ptr LTLfNot::logical_not() const { return this->get_arg(); }

LTLfNext::LTLfNext(AstManager& c, const ltlf_ptr& in)
    : LTLfFormula(c), arg_{in} {
  type_code_ = type_code_id;
}

hash_t LTLfNext::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

vec_basic LTLfNext::get_args() const {
  vec_basic v;
  v.push_back(arg_);
  return v;
}

bool LTLfNext::is_equal(const Basic& o) const {
  return is_a<LTLfNext>(o) and
         eq(*arg_, *dynamic_cast<const LTLfNext&>(o).get_arg());
}

int LTLfNext::compare_(const Basic& o) const {
  assert(is_a<LTLfNext>(o));
  return arg_->compare(*dynamic_cast<const LTLfNext&>(o).get_arg());
}

bool LTLfNext::is_canonical(const LTLfFormula& in) const { return true; }

ltlf_ptr LTLfNext::get_arg() const { return arg_; }

ltlf_ptr LTLfNext::logical_not() const { return this->get_arg(); }

LTLfWeakNext::LTLfWeakNext(AstManager& c, const ltlf_ptr& in)
    : LTLfFormula(c), arg_{in} {
  type_code_ = type_code_id;
}

hash_t LTLfWeakNext::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

vec_basic LTLfWeakNext::get_args() const {
  vec_basic v;
  v.push_back(arg_);
  return v;
}

bool LTLfWeakNext::is_equal(const Basic& o) const {
  return is_a<LTLfWeakNext>(o) and
         eq(*arg_, *dynamic_cast<const LTLfWeakNext&>(o).get_arg());
}

int LTLfWeakNext::compare_(const Basic& o) const {
  assert(is_a<LTLfWeakNext>(o));
  return arg_->compare(*dynamic_cast<const LTLfWeakNext&>(o).get_arg());
}

bool LTLfWeakNext::is_canonical(const LTLfFormula& in) const { return true; }

ltlf_ptr LTLfWeakNext::get_arg() const { return arg_; }

ltlf_ptr LTLfWeakNext::logical_not() const { return this->get_arg(); }

LTLfUntil::LTLfUntil(AstManager& c, const ltlf_ptr& arg_1,
                     const ltlf_ptr& arg_2)
    : LTLfFormula(c), arg_1_{arg_1}, arg_2_{arg_2}, container_{{arg_1, arg_2}} {
  type_code_ = type_code_id;
}

hash_t LTLfUntil::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *arg_1_);
  hash_combine<Basic>(seed, *arg_2_);
  return seed;
}

vec_ltlf_formulas LTLfUntil::get_args() const {
  vec_ltlf_formulas v;
  v.push_back(arg_1_);
  v.push_back(arg_2_);
  return v;
}

bool LTLfUntil::is_equal(const Basic& o) const {
  return is_a<LTLfUntil>(o) and
         unified_eq(get_args(), dynamic_cast<const LTLfUntil&>(o).get_args());
}

int LTLfUntil::compare_(const Basic& o) const {
  auto arg_1_compare =
      unified_compare(this->arg_1_, dynamic_cast<const LTLfUntil&>(o).arg_1_);
  if (arg_1_compare != 0)
    return arg_1_compare;
  return unified_compare(this->arg_2_,
                         dynamic_cast<const LTLfUntil&>(o).arg_2_);
}

bool LTLfUntil::is_canonical(const set_ltlf_formulas& container_) const {
  return true;
}

const set_ltlf_formulas& LTLfUntil::get_container() const { return container_; }

ltlf_ptr LTLfUntil::logical_not() const {
  auto not_arg_1 = arg_1_->logical_not();
  auto not_arg_2 = arg_2_->logical_not();
  return m_ctx->makeLtlfRelease(not_arg_1, not_arg_2);
}

LTLfRelease::LTLfRelease(AstManager& c, const ltlf_ptr& arg_1,
                         const ltlf_ptr& arg_2)
    : LTLfFormula(c), arg_1_{arg_1}, arg_2_{arg_2}, container_{{arg_1, arg_2}} {
  type_code_ = type_code_id;
}

hash_t LTLfRelease::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *arg_1_);
  hash_combine<Basic>(seed, *arg_2_);
  return seed;
}

vec_ltlf_formulas LTLfRelease::get_args() const {
  vec_ltlf_formulas v;
  v.push_back(arg_1_);
  v.push_back(arg_2_);
  return v;
}

bool LTLfRelease::is_equal(const Basic& o) const {
  return is_a<LTLfRelease>(o) and
         unified_eq(get_args(), dynamic_cast<const LTLfRelease&>(o).get_args());
}

int LTLfRelease::compare_(const Basic& o) const {
  auto arg_1_compare =
      unified_compare(this->arg_1_, dynamic_cast<const LTLfRelease&>(o).arg_1_);
  if (arg_1_compare != 0)
    return arg_1_compare;
  return unified_compare(this->arg_2_,
                         dynamic_cast<const LTLfRelease&>(o).arg_2_);
}

bool LTLfRelease::is_canonical(const set_ltlf_formulas& container_) const {
  return true;
}

const set_ltlf_formulas& LTLfRelease::get_container() const {
  return container_;
}

ltlf_ptr LTLfRelease::logical_not() const {
  auto not_arg_1 = arg_1_->logical_not();
  auto not_arg_2 = arg_2_->logical_not();
  return m_ctx->makeLtlfUntil(not_arg_1, not_arg_2);
}

LTLfEventually::LTLfEventually(AstManager& c, const ltlf_ptr& in)
    : LTLfFormula(c), arg_{in} {
  type_code_ = type_code_id;
}

hash_t LTLfEventually::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

vec_basic LTLfEventually::get_args() const {
  vec_basic v;
  v.push_back(arg_);
  return v;
}

bool LTLfEventually::is_equal(const Basic& o) const {
  return is_a<LTLfEventually>(o) and
         eq(*arg_, *dynamic_cast<const LTLfEventually&>(o).get_arg());
}

int LTLfEventually::compare_(const Basic& o) const {
  assert(is_a<LTLfEventually>(o));
  return arg_->compare(*dynamic_cast<const LTLfEventually&>(o).get_arg());
}

bool LTLfEventually::is_canonical(const LTLfFormula& in) const { return true; }

ltlf_ptr LTLfEventually::get_arg() const { return arg_; }

ltlf_ptr LTLfEventually::logical_not() const { return this->get_arg(); }

LTLfAlways::LTLfAlways(AstManager& c, const ltlf_ptr& in)
    : LTLfFormula(c), arg_{in} {
  type_code_ = type_code_id;
}

hash_t LTLfAlways::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

vec_basic LTLfAlways::get_args() const {
  vec_basic v;
  v.push_back(arg_);
  return v;
}

bool LTLfAlways::is_equal(const Basic& o) const {
  return is_a<LTLfAlways>(o) and
         eq(*arg_, *dynamic_cast<const LTLfAlways&>(o).get_arg());
}

int LTLfAlways::compare_(const Basic& o) const {
  assert(is_a<LTLfAlways>(o));
  return arg_->compare(*dynamic_cast<const LTLfAlways&>(o).get_arg());
}

bool LTLfAlways::is_canonical(const LTLfFormula& in) const { return true; }

ltlf_ptr LTLfAlways::get_arg() const { return arg_; }

ltlf_ptr LTLfAlways::logical_not() const { return this->get_arg(); }

} // namespace whitemech::lydia
