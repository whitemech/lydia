#pragma once
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

#include "basic.hpp"
#include "symbol.hpp"
#include "visitor.hpp"

namespace whitemech {
namespace lydia {

class PropositionalFormula : public Basic {
public:
  virtual prop_ptr logical_not() const = 0;
};

class PropositionalTrue : public PropositionalFormula {
public:
  const static TypeID type_code_id = TypeID::t_PropositionalTrue;
  PropositionalTrue() { this->type_code_ = type_code_id; }

  void accept(Visitor &v) const override;
  hash_t compute_hash_() const override;
  int compare(const Basic &rhs) const override;
  bool is_equal(const Basic &rhs) const override;
  prop_ptr logical_not() const override;
};

class PropositionalFalse : public PropositionalFormula {
public:
  const static TypeID type_code_id = TypeID::t_PropositionalFalse;
  PropositionalFalse() { this->type_code_ = type_code_id; }

  void accept(Visitor &v) const override;
  hash_t compute_hash_() const override;
  int compare(const Basic &rhs) const override;
  bool is_equal(const Basic &rhs) const override;
  prop_ptr logical_not() const override;
};

/*!
 *  This class is just a wrapper to a Symbol.
 */
class PropositionalAtom : public PropositionalFormula {
public:
  const static TypeID type_code_id = TypeID::t_PropositionalAtom;
  // notice, this may require downcasting when need to access to the content of
  // the symbol. we do this because PropositionalAtom can contain both Symbol
  // and QuotedFormula.
  // TODO refactor? maybe we are making complex without not good reasons.
  // an alternative: const std::variant<symbol_ptr, quoted_ptr> symbol;
  // however, the fact that we need to import from LDLF logic (for the quoted
  // formula) to implement propositional logic sounds awkward.
  const basic_ptr symbol;
  explicit PropositionalAtom(const Symbol &);
  explicit PropositionalAtom(const std::string &);
  explicit PropositionalAtom(const basic_ptr &p);
  void accept(Visitor &v) const override;
  hash_t compute_hash_() const override;
  int compare(const Basic &rhs) const override;
  bool is_equal(const Basic &rhs) const override;
  prop_ptr logical_not() const override;
};

class PropositionalAnd : public PropositionalFormula {
private:
  const set_prop_formulas container_;

public:
  const static TypeID type_code_id = TypeID::t_PropositionalAnd;
  void accept(Visitor &v) const override;
  explicit PropositionalAnd(const set_prop_formulas &s);
  bool is_canonical(const set_prop_formulas &) { return true; };
  hash_t compute_hash_() const override;
  virtual vec_prop_formulas get_args() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  const set_prop_formulas &get_container() const;
  prop_ptr logical_not() const override;
};

class PropositionalOr : public PropositionalFormula {
private:
  const set_prop_formulas container_;

public:
  const static TypeID type_code_id = TypeID::t_PropositionalOr;
  void accept(Visitor &v) const override;
  explicit PropositionalOr(const set_prop_formulas &s);
  bool is_canonical(const set_prop_formulas &container_);
  hash_t compute_hash_() const override;
  virtual vec_prop_formulas get_args() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  const set_prop_formulas &get_container() const;
  prop_ptr logical_not() const override;
};

class PropositionalNot : public PropositionalFormula {
private:
  const prop_ptr arg_;

public:
  const static TypeID type_code_id = TypeID::t_PropositionalNot;
  void accept(Visitor &v) const override;
  explicit PropositionalNot(const prop_ptr &in);
  bool is_canonical(const PropositionalFormula &s) { return true; };
  hash_t compute_hash_() const override;
  virtual vec_basic get_args() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  std::shared_ptr<const PropositionalFormula> get_arg() const;
  prop_ptr logical_not() const override;
};

extern prop_ptr prop_true;
extern prop_ptr prop_false;

prop_ptr boolean_prop(bool b);
atom_ptr prop_atom(const Symbol &s);
atom_ptr prop_atom(const std::string &s);
atom_ptr prop_atom(const basic_ptr &p);
prop_ptr logical_and(const set_prop_formulas &s);
prop_ptr logical_or(const set_prop_formulas &s);
prop_ptr logical_not(const prop_ptr &s);

} // namespace lydia
} // namespace whitemech