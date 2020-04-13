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

namespace whitemech {
namespace lydia {

class PropositionalFormula : public Basic {};

class PropositionalTrue : public PropositionalFormula {
public:
  const static TypeID type_code_id = TypeID::t_PropositionalTrue;

  void accept(Visitor &v) const override;
  hash_t __hash__() const override;
  int compare(const Basic &rhs) const override;
  bool is_equal(const Basic &rhs) const override;
};

class PropositionalFalse : public PropositionalFormula {
public:
  const static TypeID type_code_id = TypeID::t_PropositionalFalse;

  void accept(Visitor &v) const override;
  hash_t __hash__() const override;
  int compare(const Basic &rhs) const override;
  bool is_equal(const Basic &rhs) const override;
};

/*!
 *  This class is just a wrapper to a Symbol.
 */
class PropositionalAtom : public PropositionalFormula {
public:
  const Symbol symbol;
  explicit PropositionalAtom(const Symbol &);
  explicit PropositionalAtom(const std::string &);
  void accept(Visitor &v) const override;
  hash_t __hash__() const override;
  int compare(const Basic &rhs) const override;
  bool is_equal(const Basic &rhs) const override;
};

class PropositionalAnd : public PropositionalFormula {
private:
  const set_prop_formulas container_;

public:
  const static TypeID type_code_id = TypeID::t_PropositionalAnd;
  void accept(Visitor &v) const override;
  explicit PropositionalAnd(const set_prop_formulas &s);
  bool is_canonical(const set_prop_formulas &container_) { return true; };
  hash_t __hash__() const override;
  virtual vec_prop_formulas get_args() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  const set_prop_formulas &get_container() const;
};

class PropositionalOr : public PropositionalFormula {
private:
  const set_prop_formulas container_;

public:
  const static TypeID type_code_id = TypeID::t_PropositionalOr;
  void accept(Visitor &v) const override;
  explicit PropositionalOr(const set_prop_formulas &s);
  bool is_canonical(const set_prop_formulas &container_);
  hash_t __hash__() const override;
  virtual vec_prop_formulas get_args() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  const set_prop_formulas &get_container() const;
};

class PropositionalNot : public PropositionalFormula {
private:
  const std::shared_ptr<const PropositionalFormula> arg_;

public:
  const static TypeID type_code_id = TypeID::t_PropositionalNot;
  void accept(Visitor &v) const override;
  explicit PropositionalNot(
      const std::shared_ptr<const PropositionalFormula> &in);
  bool is_canonical(const PropositionalFormula &s) { return true; };
  hash_t __hash__() const override;
  virtual vec_basic get_args() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  std::shared_ptr<const PropositionalFormula> get_arg() const;
};

} // namespace lydia
} // namespace whitemech