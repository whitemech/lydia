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
#include <lydia/logic/ldlf/base.hpp>
#include <lydia/utils/compare.hpp>
#include <stdexcept>
#include <utility>

namespace whitemech::lydia {

hash_t LDLfTrue::compute_hash_() const { return type_code_id; }

vec_formulas LDLfTrue::get_args() const { return {}; }

bool LDLfTrue::is_equal(const Basic& o) const { return is_a<LDLfTrue>(o); }

int LDLfTrue::compare_(const Basic& o) const {
  assert(is_a<LDLfTrue>(o));
  return 0;
}

std::shared_ptr<const LDLfFormula> LDLfTrue::logical_not() const {
  return ctx().makeLdlfFalse();
}

///////////////////////////////////

hash_t LDLfFalse::compute_hash_() const { return type_code_id; }

vec_formulas LDLfFalse::get_args() const { return {}; }

bool LDLfFalse::is_equal(const Basic& o) const { return is_a<LDLfFalse>(o); }

int LDLfFalse::compare_(const Basic& o) const {
  assert(is_a<LDLfFalse>(o));
  return 0;
}

std::shared_ptr<const LDLfFormula> LDLfFalse::logical_not() const {
  return ctx().makeLdlfTrue();
}

LDLfAnd::LDLfAnd(AstManager& c, const set_formulas& s)
    : LDLfFormula(c), container_{s} {
  this->type_code_ = type_code_id;
  if (!is_canonical(s)) {
    throw std::invalid_argument("LDLfAnd formula: arguments must be > 1");
  }
}

hash_t LDLfAnd::compute_hash_() const {
  hash_t seed = type_code_id;
  for (const auto& a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

bool LDLfAnd::is_canonical(const set_formulas& container_) const {
  return container_.size() > 1;
}

vec_formulas LDLfAnd::get_args() const {
  vec_formulas v(container_.begin(), container_.end());
  return v;
}

bool LDLfAnd::is_equal(const Basic& o) const {
  return is_a<LDLfAnd>(o) and
         unified_eq(container_,
                    dynamic_cast<const LDLfAnd&>(o).get_container());
}

int LDLfAnd::compare_(const Basic& o) const {
  assert(is_a<LDLfAnd>(o));
  return unified_compare(container_,
                         dynamic_cast<const LDLfAnd&>(o).get_container());
}

const set_formulas& LDLfAnd::get_container() const { return container_; }

std::shared_ptr<const LDLfFormula> LDLfAnd::logical_not() const {
  auto container = this->get_container();
  set_formulas cont;
  for (auto& a : container) {
    cont.insert(a->logical_not());
  }
  return m_ctx->makeLdlfOr(cont);
}

LDLfOr::LDLfOr(AstManager& c, const set_formulas& s)
    : LDLfFormula(c), container_{s} {
  this->type_code_ = type_code_id;
  if (!is_canonical(s)) {
    throw std::invalid_argument("LDLfOr formula: arguments must be > 1");
  }
}

hash_t LDLfOr::compute_hash_() const {
  hash_t seed = type_code_id;
  for (const auto& a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

vec_formulas LDLfOr::get_args() const {
  vec_formulas v(container_.begin(), container_.end());
  return v;
}

bool LDLfOr::is_equal(const Basic& o) const {
  return is_a<LDLfOr>(o) and
         unified_eq(container_, dynamic_cast<const LDLfOr&>(o).get_container());
}

int LDLfOr::compare_(const Basic& o) const {
  assert(is_a<LDLfOr>(o));
  return unified_compare(container_,
                         dynamic_cast<const LDLfOr&>(o).get_container());
}

bool LDLfOr::is_canonical(const set_formulas& container_) const {
  return container_.size() > 1;
}

const set_formulas& LDLfOr::get_container() const { return container_; }

std::shared_ptr<const LDLfFormula> LDLfOr::logical_not() const {
  auto container = this->get_container();
  set_formulas cont;
  for (auto& a : container) {
    cont.insert(a->logical_not());
  }
  return m_ctx->makeLdlfAnd(cont);
}

LDLfNot::LDLfNot(AstManager& c, const std::shared_ptr<const LDLfFormula>& in)
    : LDLfFormula(c), arg_{in} {
  type_code_ = type_code_id;
}

hash_t LDLfNot::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

vec_basic LDLfNot::get_args() const {
  vec_basic v;
  v.push_back(arg_);
  return v;
}

bool LDLfNot::is_equal(const Basic& o) const {
  return is_a<LDLfNot>(o) and
         eq(*arg_, *dynamic_cast<const LDLfNot&>(o).get_arg());
}

int LDLfNot::compare_(const Basic& o) const {
  assert(is_a<LDLfNot>(o));
  return arg_->compare(*dynamic_cast<const LDLfNot&>(o).get_arg());
}

bool LDLfNot::is_canonical(const LDLfFormula& in) const { return true; }

std::shared_ptr<const LDLfFormula> LDLfNot::get_arg() const { return arg_; }

std::shared_ptr<const LDLfFormula> LDLfNot::logical_not() const {
  return this->get_arg();
}

LDLfDiamond::LDLfDiamond(AstManager& c, const regex_ptr& regex,
                         const ldlf_ptr& formula)
    : LDLfTemporal(c, regex, formula) {
  this->type_code_ = type_code_id;
}

hash_t LDLfDiamond::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *this->get_regex());
  hash_combine<Basic>(seed, *this->get_formula());
  return seed;
};

bool LDLfDiamond::is_canonical(const set_formulas& container_) const {
  // TODO
  return true;
}

bool LDLfDiamond::is_equal(const Basic& o) const {
  return is_a<LDLfDiamond>(o) and
         unified_eq(this->get_regex(),
                    dynamic_cast<const LDLfDiamond&>(o).get_regex()) and
         unified_eq(this->get_formula(),
                    dynamic_cast<const LDLfDiamond&>(o).get_formula());
}

int LDLfDiamond::compare_(const Basic& o) const {
  auto regex_compare = unified_compare(
      this->get_regex(), dynamic_cast<const LDLfDiamond&>(o).get_regex());
  if (regex_compare != 0)
    return regex_compare;
  return unified_compare(this->get_formula(),
                         dynamic_cast<const LDLfDiamond&>(o).get_formula());
}

std::shared_ptr<const LDLfFormula> LDLfDiamond::logical_not() const {
  return ctx().makeLdlfBox(this->get_regex(),
                           this->get_formula()->logical_not());
}

LDLfBox::LDLfBox(AstManager& c, const regex_ptr& regex, const ldlf_ptr& formula)
    : LDLfTemporal(c, regex, formula) {
  this->type_code_ = type_code_id;
}

hash_t LDLfBox::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *this->get_regex());
  hash_combine<Basic>(seed, *this->get_formula());
  return seed;
}

bool LDLfBox::is_canonical(const set_formulas& container_) const {
  // TODO
  return true;
}
bool LDLfBox::is_equal(const Basic& o) const {
  return is_a<LDLfBox>(o) and
         unified_eq(this->get_regex(),
                    dynamic_cast<const LDLfBox&>(o).get_regex()) and
         unified_eq(this->get_formula(),
                    dynamic_cast<const LDLfBox&>(o).get_formula());
}

int LDLfBox::compare_(const Basic& o) const {
  auto regex_compare = unified_compare(
      this->get_regex(), dynamic_cast<const LDLfBox&>(o).get_regex());
  if (regex_compare != 0)
    return regex_compare;
  return unified_compare(this->get_formula(),
                         dynamic_cast<const LDLfBox&>(o).get_formula());
}

std::shared_ptr<const LDLfFormula> LDLfBox::logical_not() const {
  return ctx().makeLdlfDiamond(this->get_regex(),
                               this->get_formula()->logical_not());
}

PropositionalRegExp::PropositionalRegExp(
    AstManager& c, std::shared_ptr<const PropositionalFormula> f)
    : RegExp(c), arg_{std::move(f)} {
  this->type_code_ = type_code_id;
}

hash_t PropositionalRegExp::compute_hash_() const {
  hash_t seed = TypeID::t_PropositionalRegExp;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

std::shared_ptr<const PropositionalFormula>
PropositionalRegExp::get_arg() const {
  return arg_;
}

bool PropositionalRegExp::is_equal(const Basic& o) const {
  return is_a<PropositionalRegExp>(o) and
         eq(*arg_, *dynamic_cast<const PropositionalRegExp&>(o).get_arg());
}

int PropositionalRegExp::compare_(const Basic& o) const {
  assert(is_a<PropositionalRegExp>(o));
  return arg_->compare(*dynamic_cast<const PropositionalRegExp&>(o).get_arg());
}

bool PropositionalRegExp::is_canonical(const PropositionalFormula& f) const {
  // TODO
  return true;
}

TestRegExp::TestRegExp(AstManager& c, std::shared_ptr<const LDLfFormula> f)
    : RegExp(c), arg_{std::move(f)} {
  this->type_code_ = type_code_id;
}

hash_t TestRegExp::compute_hash_() const {
  hash_t seed = TypeID::t_TestRegExp;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

std::shared_ptr<const LDLfFormula> TestRegExp::get_arg() const { return arg_; }

bool TestRegExp::is_equal(const Basic& o) const {
  return is_a<TestRegExp>(o) and
         eq(*arg_, *dynamic_cast<const TestRegExp&>(o).get_arg());
}

int TestRegExp::compare_(const Basic& o) const {
  assert(is_a<TestRegExp>(o));
  return arg_->compare(*dynamic_cast<const TestRegExp&>(o).get_arg());
}

bool TestRegExp::is_canonical(const LDLfFormula& f) const {
  // TODO
  return true;
}

UnionRegExp::UnionRegExp(AstManager& c, const set_regex& args)
    : RegExp(c), container_{args} {
  this->type_code_ = type_code_id;
}

bool UnionRegExp::is_canonical(const set_regex& args) const { return true; }

hash_t UnionRegExp::compute_hash_() const {
  hash_t seed = TypeID::t_UnionRegExp;
  for (const auto& a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

const set_regex& UnionRegExp::get_container() const { return container_; }

bool UnionRegExp::is_equal(const Basic& o) const {
  return is_a<UnionRegExp>(o) and
         unified_eq(container_,
                    dynamic_cast<const UnionRegExp&>(o).get_container());
}

int UnionRegExp::compare_(const Basic& o) const {
  assert(is_a<UnionRegExp>(o));
  return unified_compare(container_,
                         dynamic_cast<const UnionRegExp&>(o).get_container());
}

SequenceRegExp::SequenceRegExp(AstManager& c, const vec_regex& args)
    : RegExp(c), container_{std::move(args)} {
  this->type_code_ = type_code_id;
}

bool SequenceRegExp::is_canonical(const set_regex& args) const { return true; }

hash_t SequenceRegExp::compute_hash_() const {
  hash_t seed = TypeID::t_SequenceRegExp;
  for (const auto& a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

const vec_regex& SequenceRegExp::get_container() const { return container_; }

bool SequenceRegExp::is_equal(const Basic& o) const {
  return is_a<SequenceRegExp>(o) and
         unified_eq(container_,
                    dynamic_cast<const SequenceRegExp&>(o).get_container());
}

int SequenceRegExp::compare_(const Basic& o) const {
  assert(is_a<SequenceRegExp>(o));
  return unified_compare(
      container_, dynamic_cast<const SequenceRegExp&>(o).get_container());
}

StarRegExp::StarRegExp(AstManager& c, regex_ptr arg)
    : RegExp(c), arg_{std::move(arg)} {
  this->type_code_ = type_code_id;
}

bool StarRegExp::is_canonical(const set_regex& args) const { return true; }

hash_t StarRegExp::compute_hash_() const {
  hash_t seed = TypeID::t_StarRegExp;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

const regex_ptr& StarRegExp::get_arg() const { return arg_; }

bool StarRegExp::is_equal(const Basic& o) const {
  return is_a<StarRegExp>(o) and
         eq(*arg_, *dynamic_cast<const StarRegExp&>(o).get_arg());
}

int StarRegExp::compare_(const Basic& o) const {
  assert(is_a<StarRegExp>(o));
  return arg_->compare(*dynamic_cast<const StarRegExp&>(o).get_arg());
}

LDLfF::LDLfF(AstManager& c, const ldlf_ptr& formula)
    : LDLfFormula(c), arg_{formula} {
  this->type_code_ = type_code_id;
}

hash_t LDLfF::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

bool LDLfF::is_canonical(const set_regex& args) const { return true; }

ldlf_ptr LDLfF::logical_not() const {
  return ctx().makeLdlfT(this->get_arg()->logical_not());
}

ldlf_ptr LDLfF::get_arg() const { return this->arg_; }

bool LDLfF::is_equal(const Basic& rhs) const {
  return is_a<LDLfF>(rhs) and
         eq(*arg_, *dynamic_cast<const LDLfF&>(rhs).get_arg());
}

int LDLfF::compare_(const Basic& rhs) const {
  assert(is_a<LDLfF>(rhs));
  return arg_->compare(*dynamic_cast<const LDLfF&>(rhs).get_arg());
}

LDLfT::LDLfT(AstManager& c, const ldlf_ptr& formula)
    : LDLfFormula(c), arg_{formula} {
  this->type_code_ = type_code_id;
}

hash_t LDLfT::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

bool LDLfT::is_canonical(const set_regex& args) const { return true; }

ldlf_ptr LDLfT::get_arg() const { return this->arg_; }

ldlf_ptr LDLfT::logical_not() const {
  return m_ctx->makeLdlfF(this->get_arg()->logical_not());
}

bool LDLfT::is_equal(const Basic& rhs) const {
  return is_a<LDLfT>(rhs) and
         eq(*arg_, *dynamic_cast<const LDLfT&>(rhs).get_arg());
}

int LDLfT::compare_(const Basic& rhs) const {
  assert(is_a<LDLfT>(rhs));
  return arg_->compare(*dynamic_cast<const LDLfT&>(rhs).get_arg());
}

LDLfQ::LDLfQ(AstManager& c, const ldlf_ptr& formula)
    : LDLfFormula(c), arg_{formula} {
  this->type_code_ = type_code_id;
}

hash_t LDLfQ::compute_hash_() const {
  hash_t seed = type_code_id;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

bool LDLfQ::is_canonical(const set_regex& args) const { return true; }

ldlf_ptr LDLfQ::get_arg() const { return this->arg_; }

ldlf_ptr LDLfQ::logical_not() const {
  return m_ctx->makeLdlfF(this->get_arg()->logical_not());
}

bool LDLfQ::is_equal(const Basic& rhs) const {
  return is_a<LDLfQ>(rhs) and
         eq(*arg_, *dynamic_cast<const LDLfQ&>(rhs).get_arg());
}

int LDLfQ::compare_(const Basic& rhs) const {
  assert(is_a<LDLfQ>(rhs));
  return arg_->compare(*dynamic_cast<const LDLfQ&>(rhs).get_arg());
}

QuotedFormula::QuotedFormula(basic_ptr formula) : formula{std::move(formula)} {
  this->type_code_ = type_code_id;
}

hash_t QuotedFormula::compute_hash_() const {
  hash_t seed = TypeID::t_QuotedFormula;
  hash_combine<Basic>(seed, *this->formula);
  return seed;
}

int QuotedFormula::compare_(const Basic& rhs) const {
  assert(is_a<QuotedFormula>(rhs));
  return this->formula->compare(
      *dynamic_cast<const QuotedFormula&>(rhs).formula);
}

bool QuotedFormula::is_equal(const Basic& rhs) const {
  return is_a<QuotedFormula>(rhs) and
         eq(*formula, *dynamic_cast<const QuotedFormula&>(rhs).formula);
}

std::shared_ptr<const QuotedFormula> quote(const basic_ptr& p) {
  return std::make_shared<QuotedFormula>(p);
}

} // namespace whitemech::lydia
