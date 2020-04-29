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

const std::shared_ptr<const LDLfBooleanAtom> boolTrue =
    std::make_shared<const LDLfBooleanAtom>(true);
const std::shared_ptr<const LDLfBooleanAtom> boolFalse =
    std::make_shared<const LDLfBooleanAtom>(false);

LDLfBooleanAtom::LDLfBooleanAtom(bool b) : b_{b} {
  this->type_code_ = type_code_id;
}

hash_t LDLfBooleanAtom::compute_hash_() const {
  return b_ ? TypeID::t_LDLfBooleanTrue : TypeID::t_LDLfBooleanFalse;
}

bool LDLfBooleanAtom::get_value() const { return b_; }

vec_formulas LDLfBooleanAtom::get_args() const { return {}; }

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

std::shared_ptr<const LDLfFormula> LDLfBooleanAtom::logical_not() const {
  return boolean(not this->get_value());
}

LDLfAnd::LDLfAnd(const set_formulas &s) : container_{s} {
  this->type_code_ = type_code_id;
  if (!is_canonical(s)) {
    throw std::invalid_argument("LDLfAnd formula: arguments must be > 1");
  }
}

hash_t LDLfAnd::compute_hash_() const {
  hash_t seed = TypeID::t_LDLfAnd;
  for (const auto &a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

bool LDLfAnd::is_canonical(const set_formulas &container_) const {
  return container_.size() > 1;
}

vec_formulas LDLfAnd::get_args() const {
  vec_formulas v(container_.begin(), container_.end());
  return v;
}

bool LDLfAnd::is_equal(const Basic &o) const {
  return is_a<LDLfAnd>(o) and
         unified_eq(container_,
                    dynamic_cast<const LDLfAnd &>(o).get_container());
}

int LDLfAnd::compare(const Basic &o) const {
  assert(is_a<LDLfAnd>(o));
  return unified_compare(container_,
                         dynamic_cast<const LDLfAnd &>(o).get_container());
}

const set_formulas &LDLfAnd::get_container() const { return container_; }

std::shared_ptr<const LDLfFormula> LDLfAnd::logical_not() const {
  auto container = this->get_container();
  set_formulas cont;
  for (auto &a : container) {
    cont.insert(a->logical_not());
  }
  return std::make_shared<LDLfOr>(cont);
}

LDLfOr::LDLfOr(const set_formulas &s) : container_{s} {
  this->type_code_ = type_code_id;
  if (!is_canonical(s)) {
    throw std::invalid_argument("LDLfOr formula: arguments must be > 1");
  }
}

hash_t LDLfOr::compute_hash_() const {
  hash_t seed = TypeID::t_LDLfOr;
  for (const auto &a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

vec_formulas LDLfOr::get_args() const {
  vec_formulas v(container_.begin(), container_.end());
  return v;
}

bool LDLfOr::is_equal(const Basic &o) const {
  return is_a<LDLfOr>(o) and
         unified_eq(container_,
                    dynamic_cast<const LDLfOr &>(o).get_container());
}

int LDLfOr::compare(const Basic &o) const {
  assert(is_a<LDLfOr>(o));
  return unified_compare(container_,
                         dynamic_cast<const LDLfOr &>(o).get_container());
}

bool LDLfOr::is_canonical(const set_formulas &container_) const {
  return container_.size() > 1;
}

const set_formulas &LDLfOr::get_container() const { return container_; }

std::shared_ptr<const LDLfFormula> LDLfOr::logical_not() const {
  auto container = this->get_container();
  set_formulas cont;
  for (auto &a : container) {
    cont.insert(a->logical_not());
  }
  return std::make_shared<LDLfAnd>(cont);
}

LDLfNot::LDLfNot(const std::shared_ptr<const LDLfFormula> &in) : arg_{in} {
  this->type_code_ = type_code_id;
  if (!is_canonical(*in)) {
    throw std::invalid_argument("LDLfNot formula: argument cannot be an "
                                "LDLfBooleanAtom or an LDLfNot");
  }
}

hash_t LDLfNot::compute_hash_() const {
  hash_t seed = TypeID::t_LDLfNot;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

vec_basic LDLfNot::get_args() const {
  vec_basic v;
  v.push_back(arg_);
  return v;
}

bool LDLfNot::is_equal(const Basic &o) const {
  return is_a<LDLfNot>(o) and
         eq(*arg_, *dynamic_cast<const LDLfNot &>(o).get_arg());
}

int LDLfNot::compare(const Basic &o) const {
  assert(is_a<LDLfNot>(o));
  return arg_->compare_(*dynamic_cast<const LDLfNot &>(o).get_arg());
}

bool LDLfNot::is_canonical(const LDLfFormula &in) const {
  // TODO add is_a<LDLfBooleanAtom>(in) when we will have other LDLf formulas
  return !(is_a<LDLfNot>(in));
}

std::shared_ptr<const LDLfFormula> LDLfNot::get_arg() const { return arg_; }

std::shared_ptr<const LDLfFormula> LDLfNot::logical_not() const {
  return this->get_arg();
}

hash_t LDLfTemporal::compute_hash_() const {
  hash_t seed = this->get_type_code();
  hash_combine<Basic>(seed, *this->get_regex());
  hash_combine<Basic>(seed, *this->get_formula());
  return seed;
};

LDLfDiamond::LDLfDiamond(const regex_ptr &regex, const ldlf_ptr &formula)
    : LDLfTemporal(regex, formula) {
  this->type_code_ = type_code_id;
}

bool LDLfDiamond::is_canonical(const set_formulas &container_) const {
  // TODO
  return true;
}

bool LDLfDiamond::is_equal(const Basic &o) const {
  return is_a<LDLfDiamond>(o) and
         unified_eq(this->get_regex(),
                    dynamic_cast<const LDLfDiamond &>(o).get_regex()) and
         unified_eq(this->get_formula(),
                    dynamic_cast<const LDLfDiamond &>(o).get_formula());
}

int LDLfDiamond::compare(const Basic &o) const {
  auto regex_compare = unified_compare(
      this->get_regex(), dynamic_cast<const LDLfDiamond &>(o).get_regex());
  if (regex_compare != 0)
    return regex_compare;
  return unified_compare(this->get_formula(),
                         dynamic_cast<const LDLfDiamond &>(o).get_formula());
}

std::shared_ptr<const LDLfFormula> LDLfDiamond::logical_not() const {
  return std::make_shared<LDLfBox>(this->get_regex(),
                                   this->get_formula()->logical_not());
};

LDLfBox::LDLfBox(const regex_ptr &regex, const ldlf_ptr &formula)
    : LDLfTemporal(regex, formula) {
  this->type_code_ = type_code_id;
}

bool LDLfBox::is_canonical(const set_formulas &container_) const {
  // TODO
  return true;
}
bool LDLfBox::is_equal(const Basic &o) const {
  return is_a<LDLfBox>(o) and
         unified_eq(this->get_regex(),
                    dynamic_cast<const LDLfBox &>(o).get_regex()) and
         unified_eq(this->get_formula(),
                    dynamic_cast<const LDLfBox &>(o).get_formula());
}

int LDLfBox::compare(const Basic &o) const {
  auto regex_compare = unified_compare(
      this->get_regex(), dynamic_cast<const LDLfBox &>(o).get_regex());
  if (regex_compare != 0)
    return regex_compare;
  return unified_compare(this->get_formula(),
                         dynamic_cast<const LDLfBox &>(o).get_formula());
}

std::shared_ptr<const LDLfFormula> LDLfBox::logical_not() const {
  return std::make_shared<LDLfDiamond>(this->get_regex(),
                                       this->get_formula()->logical_not());
}

PropositionalRegExp::PropositionalRegExp(
    std::shared_ptr<const PropositionalFormula> f)
    : arg_{std::move(f)} {
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

bool PropositionalRegExp::is_equal(const Basic &o) const {
  return is_a<PropositionalRegExp>(o) and
         eq(*arg_, *dynamic_cast<const PropositionalRegExp &>(o).get_arg());
}

int PropositionalRegExp::compare(const Basic &o) const {
  assert(is_a<PropositionalRegExp>(o));
  return arg_->compare_(
      *dynamic_cast<const PropositionalRegExp &>(o).get_arg());
}

bool PropositionalRegExp::is_canonical(const PropositionalFormula &f) const {
  // TODO
  return true;
}

TestRegExp::TestRegExp(std::shared_ptr<const LDLfFormula> f)
    : arg_{std::move(f)} {
  this->type_code_ = type_code_id;
}

hash_t TestRegExp::compute_hash_() const {
  hash_t seed = TypeID::t_TestRegExp;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

std::shared_ptr<const LDLfFormula> TestRegExp::get_arg() const { return arg_; }

bool TestRegExp::is_equal(const Basic &o) const {
  return is_a<TestRegExp>(o) and
         eq(*arg_, *dynamic_cast<const TestRegExp &>(o).get_arg());
}

int TestRegExp::compare(const Basic &o) const {
  assert(is_a<TestRegExp>(o));
  return arg_->compare_(*dynamic_cast<const TestRegExp &>(o).get_arg());
}

bool TestRegExp::is_canonical(const LDLfFormula &f) const {
  // TODO
  return true;
}

UnionRegExp::UnionRegExp(const set_regex &args) : container_{args} {
  this->type_code_ = type_code_id;
}

bool UnionRegExp::is_canonical(const set_regex &args) const { return true; }

hash_t UnionRegExp::compute_hash_() const {
  hash_t seed = TypeID::t_UnionRegExp;
  for (const auto &a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

const set_regex &UnionRegExp::get_container() const { return container_; }

bool UnionRegExp::is_equal(const Basic &o) const {
  return is_a<UnionRegExp>(o) and
         unified_eq(container_,
                    dynamic_cast<const UnionRegExp &>(o).get_container());
}

int UnionRegExp::compare(const Basic &o) const {
  assert(is_a<UnionRegExp>(o));
  return unified_compare(container_,
                         dynamic_cast<const UnionRegExp &>(o).get_container());
}

SequenceRegExp::SequenceRegExp(const vec_regex &args)
    : container_{std::move(args)} {
  this->type_code_ = type_code_id;
}

bool SequenceRegExp::is_canonical(const set_regex &args) const { return true; }

hash_t SequenceRegExp::compute_hash_() const {
  hash_t seed = TypeID::t_SequenceRegExp;
  for (const auto &a : container_)
    hash_combine<Basic>(seed, *a);
  return seed;
}

const vec_regex &SequenceRegExp::get_container() const { return container_; }

bool SequenceRegExp::is_equal(const Basic &o) const {
  return is_a<SequenceRegExp>(o) and
         unified_eq(container_,
                    dynamic_cast<const SequenceRegExp &>(o).get_container());
}

int SequenceRegExp::compare(const Basic &o) const {
  assert(is_a<SequenceRegExp>(o));
  return unified_compare(
      container_, dynamic_cast<const SequenceRegExp &>(o).get_container());
}

StarRegExp::StarRegExp(regex_ptr arg) : arg_{std::move(arg)} {
  this->type_code_ = type_code_id;
}

bool StarRegExp::is_canonical(const set_regex &args) const { return true; }

hash_t StarRegExp::compute_hash_() const {
  hash_t seed = TypeID::t_StarRegExp;
  hash_combine<Basic>(seed, *arg_);
  return seed;
}

const regex_ptr &StarRegExp::get_arg() const { return arg_; }

bool StarRegExp::is_equal(const Basic &o) const {
  return is_a<StarRegExp>(o) and
         eq(*arg_, *dynamic_cast<const StarRegExp &>(o).get_arg());
}

int StarRegExp::compare(const Basic &o) const {
  assert(is_a<StarRegExp>(o));
  return arg_->compare_(*dynamic_cast<const StarRegExp &>(o).get_arg());
}

LDLfF::LDLfF(const LDLfFormula &formula) : arg_{formula} {
  this->type_code_ = type_code_id;
}

hash_t LDLfF::compute_hash_() const {
  hash_t seed = TypeID::t_LDLfF;
  hash_combine<Basic>(seed, *this);
  return seed;
}

bool LDLfF::is_canonical(const set_regex &args) const { return true; }

ldlf_ptr LDLfF::logical_not() const {
  return std::make_shared<LDLfT>(*this->get_arg().logical_not());
}

const LDLfFormula &LDLfF::get_arg() const { return this->arg_; }

bool LDLfF::is_equal(const Basic &rhs) const {
  return is_a<LDLfF>(rhs) and
         eq(arg_, dynamic_cast<const LDLfF &>(rhs).get_arg());
}

int LDLfF::compare(const Basic &rhs) const {
  assert(is_a<LDLfF>(rhs));
  return arg_.compare_(dynamic_cast<const LDLfF &>(rhs).get_arg());
}

LDLfT::LDLfT(const LDLfFormula &formula) : arg_{formula} {
  this->type_code_ = type_code_id;
}

hash_t LDLfT::compute_hash_() const {
  hash_t seed = TypeID::t_LDLfT;
  hash_combine<Basic>(seed, *this);
  return seed;
}

bool LDLfT::is_canonical(const set_regex &args) const { return true; }

const LDLfFormula &LDLfT::get_arg() const { return this->arg_; }

ldlf_ptr LDLfT::logical_not() const {
  return std::make_shared<LDLfF>(*this->get_arg().logical_not());
}

bool LDLfT::is_equal(const Basic &rhs) const {
  return is_a<LDLfT>(rhs) and
         eq(arg_, dynamic_cast<const LDLfT &>(rhs).get_arg());
}

int LDLfT::compare(const Basic &rhs) const {
  assert(is_a<LDLfT>(rhs));
  return arg_.compare_(dynamic_cast<const LDLfT &>(rhs).get_arg());
}

QuotedFormula::QuotedFormula(ldlf_ptr formula) : formula{std::move(formula)} {
  this->type_code_ = type_code_id;
}

hash_t QuotedFormula::compute_hash_() const {
  hash_t seed = TypeID::t_QuotedFormula;
  hash_combine<Basic>(seed, *this->formula);
  return seed;
}

int QuotedFormula::compare(const Basic &rhs) const {
  assert(is_a<QuotedFormula>(rhs));
  return this->formula->compare_(
      *dynamic_cast<const QuotedFormula &>(rhs).formula);
}

bool QuotedFormula::is_equal(const Basic &rhs) const {
  return is_a<QuotedFormula>(rhs) and
         eq(*formula, *dynamic_cast<const QuotedFormula &>(rhs).formula);
}

std::shared_ptr<const QuotedFormula> quote(const ldlf_ptr &p) {
  return std::make_shared<QuotedFormula>(p);
}

} // namespace lydia
} // namespace whitemech
