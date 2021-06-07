#pragma once
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

#include <lydia/ast/base.hpp>
#include <lydia/basic.hpp>
#include <lydia/logic/symbol.hpp>
#include <lydia/visitor.hpp>

namespace whitemech::lydia {

class PropositionalFormula : public Ast {
public:
  virtual prop_ptr logical_not() const = 0;
  explicit PropositionalFormula(AstManager& c) : Ast(c) {}
  virtual set_prop_formulas get_container() const = 0;
};

class PropositionalTrue : public PropositionalFormula {
public:
  const static TypeID type_code_id = TypeID::t_PropositionalTrue;
  explicit PropositionalTrue(AstManager& c) : PropositionalFormula(c) {
    this->type_code_ = type_code_id;
  }
  void accept(Visitor& v) const override;
  hash_t compute_hash_() const override;
  int compare_(const Basic& rhs) const override;
  bool is_equal(const Basic& rhs) const override;
  prop_ptr logical_not() const override;
  set_prop_formulas get_container() const override {
    return set_prop_formulas{};
  };
};

class PropositionalFalse : public PropositionalFormula {
public:
  const static TypeID type_code_id = TypeID::t_PropositionalFalse;
  explicit PropositionalFalse(AstManager& c) : PropositionalFormula(c) {
    this->type_code_ = type_code_id;
  }
  void accept(Visitor& v) const override;
  hash_t compute_hash_() const override;
  int compare_(const Basic& rhs) const override;
  bool is_equal(const Basic& rhs) const override;
  prop_ptr logical_not() const override;
  set_prop_formulas get_container() const override {
    return set_prop_formulas{};
  };
};

/*!
 *  This class is just a wrapper to a Symbol.
 */
class PropositionalAtom : public PropositionalFormula {
public:
  const static TypeID type_code_id = TypeID::t_PropositionalAtom;
  // We use basic_ptr because PropositionalAtom can contain both Symbol and
  // QuotedFormula.
  // TODO: use variant
  const basic_ptr symbol;
  explicit PropositionalAtom(AstManager& c, const std::string&);
  explicit PropositionalAtom(AstManager& c, basic_ptr p);
  void accept(Visitor& v) const override;
  hash_t compute_hash_() const override;
  int compare_(const Basic& rhs) const override;
  bool is_equal(const Basic& rhs) const override;
  prop_ptr logical_not() const override;
  // TODO find a proper solution. Either:
  //   - enable_shared_from_this
  //   - ask the ast manager to return a shared pointer from a reference (using
  //   the hash?)
  //   - re-instantiate the propositional atom (bleah)
  set_prop_formulas get_container() const override {
    return set_prop_formulas{};
  };
};

class PropositionalAnd : public PropositionalFormula {
private:
  const set_prop_formulas container_;

public:
  const static TypeID type_code_id = TypeID::t_PropositionalAnd;
  void accept(Visitor& v) const override;
  explicit PropositionalAnd(AstManager& c, const set_prop_formulas& s);
  bool is_canonical(const set_prop_formulas&) { return true; };
  hash_t compute_hash_() const override;
  virtual vec_prop_formulas get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  set_prop_formulas get_container() const override;
  prop_ptr logical_not() const override;
};

class PropositionalOr : public PropositionalFormula {
private:
  const set_prop_formulas container_;

public:
  const static TypeID type_code_id = TypeID::t_PropositionalOr;
  void accept(Visitor& v) const override;
  explicit PropositionalOr(AstManager& c, const set_prop_formulas& s);
  bool is_canonical(const set_prop_formulas& container_);
  hash_t compute_hash_() const override;
  virtual vec_prop_formulas get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  set_prop_formulas get_container() const override;
  prop_ptr logical_not() const override;
};

class PropositionalNot : public PropositionalFormula {
private:
  const prop_ptr arg_;

public:
  const static TypeID type_code_id = TypeID::t_PropositionalNot;
  void accept(Visitor& v) const override;
  explicit PropositionalNot(AstManager& c, const prop_ptr& in);
  bool is_canonical(const PropositionalFormula& s) { return true; };
  hash_t compute_hash_() const override;
  virtual vec_basic get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  std::shared_ptr<const PropositionalFormula> get_arg() const;
  prop_ptr logical_not() const override;
  set_prop_formulas get_container() const override;
};

} // namespace whitemech::lydia