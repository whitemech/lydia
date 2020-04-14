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

class LDLfFormula : public Basic {
public:
  virtual std::shared_ptr<const LDLfFormula> logical_not() const = 0;
};

// Temporal True and False (tt and ff)
// TODO this could be split in two classes: LDLfTrue and LDLfFalse.
class LDLfBooleanAtom : public LDLfFormula {

private:
  bool b_;

public:
  const static TypeID type_code_id = TypeID::t_LDLfBooleanAtom;
  void accept(Visitor &v) const override;
  explicit LDLfBooleanAtom(bool b);
  hash_t __hash__() const override;
  bool get_value() const;
  virtual vec_formulas get_args() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  std::shared_ptr<const LDLfFormula> logical_not() const override;
};

extern const std::shared_ptr<const LDLfBooleanAtom> boolTrue;
extern const std::shared_ptr<const LDLfBooleanAtom> boolFalse;

inline std::shared_ptr<const LDLfBooleanAtom> boolean(bool b) {
  return b ? boolTrue : boolFalse;
}

class LDLfAnd : public LDLfFormula {
private:
  const set_formulas container_;

public:
  const static TypeID type_code_id = TypeID::t_LDLfAnd;
  void accept(Visitor &v) const override;
  explicit LDLfAnd(const set_formulas &s);
  bool is_canonical(const set_formulas &container_);
  hash_t __hash__() const override;
  virtual vec_formulas get_args() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  const set_formulas &get_container() const;
  std::shared_ptr<const LDLfFormula> logical_not() const override;
};

class LDLfOr : public LDLfFormula {
private:
  const set_formulas container_;

public:
  const static TypeID type_code_id = TypeID::t_LDLfOr;
  void accept(Visitor &v) const override;
  explicit LDLfOr(const set_formulas &s);
  bool is_canonical(const set_formulas &container_); // TODO
  hash_t __hash__() const override;
  virtual vec_formulas get_args() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  const set_formulas &get_container() const;
  std::shared_ptr<const LDLfFormula> logical_not() const override;
};

class LDLfNot : public LDLfFormula {
private:
  const std::shared_ptr<const LDLfFormula> arg_;

public:
  const static TypeID type_code_id = TypeID::t_LDLfNot;
  void accept(Visitor &v) const override;
  explicit LDLfNot(const std::shared_ptr<const LDLfFormula> &in);
  bool is_canonical(const LDLfFormula &s);
  hash_t __hash__() const override;
  virtual vec_basic get_args() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  std::shared_ptr<const LDLfFormula> get_arg() const;
  std::shared_ptr<const LDLfFormula> logical_not() const override;
};

class QuotedFormula : public Basic {
private:
protected:
public:
  const static TypeID type_code_id = TypeID::t_QuotedFormula;
  const std::shared_ptr<LDLfFormula> formula;

  /*!
   * Quote an LDLf formula. We assume it is in NNF.
   * @param f: the LDLf formula.
   */
  explicit QuotedFormula(std::shared_ptr<LDLfFormula> formula)
      : formula{std::move(formula)} {
    this->type_code_ = TypeID::t_QuotedFormula;
  }

  void accept(Visitor &v) const override;
  hash_t __hash__() const override;
  int compare(const Basic &rhs) const override;
  bool is_equal(const Basic &rhs) const override;
};

} // namespace lydia
} // namespace whitemech
