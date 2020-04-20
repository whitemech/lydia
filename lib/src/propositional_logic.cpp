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
#include <atom_visitor.hpp>
#include <cassert>
#include <propositional_logic.hpp>
#include <utils/compare.hpp>
#include <utils/misc.hpp>

namespace whitemech {
namespace lydia {

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

PropositionalAtom::PropositionalAtom(const Symbol &s)
    : symbol{std::make_shared<const Symbol>(s.get_name())} {
  this->type_code_ = type_code_id;
}

PropositionalAtom::PropositionalAtom(std::shared_ptr<const Basic> &p)
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

void EvalVisitor::visit(const PropositionalTrue &) { result = true; }
void EvalVisitor::visit(const PropositionalFalse &) { result = false; }

void EvalVisitor::visit(const PropositionalAtom &a) {
  for (const auto &x : interpretation) {
    if (*x == a) {
      result = true;
      return;
    }
  }
  result = false;
}

void EvalVisitor::visit(const PropositionalNot &a) {
  result = not apply(*a.get_arg());
}

void EvalVisitor::visit(const PropositionalAnd &a) {
  result = true;
  for (const auto &subformula : a.get_container()) {
    if (!apply(*subformula)) {
      result = false;
      return;
    }
  }
}

void EvalVisitor::visit(const PropositionalOr &a) {
  result = false;
  for (const auto &subformula : a.get_container()) {
    if (apply(*subformula)) {
      result = true;
      return;
    }
  }
}

bool EvalVisitor::apply(const PropositionalFormula &b) {
  b.accept(*this);
  return result;
}

bool eval(const PropositionalFormula &f, set_atoms_ptr &interpretation) {
  EvalVisitor evalVisitor{interpretation};
  return evalVisitor.apply(f);
}

std::vector<set_atoms_ptr> all_models(const PropositionalFormula &f) {
  std::vector<set_atoms_ptr> models;
  auto all_atoms = find_atoms(f);
  std::vector<set_atoms_ptr> all_interpretations =
      powerset<atom_ptr, SharedComparator>(all_atoms);
  for (set_atoms_ptr &interpretation : all_interpretations) {
    if (eval(f, interpretation)) {
      models.emplace_back(interpretation);
    }
  }
  return models;
}

std::vector<set_atoms_ptr> minimal_models(const PropositionalFormula &f) {
  auto models = all_models(f);
  std::vector<set_atoms_ptr> result;
  return models;
}

} // namespace lydia
} // namespace whitemech
