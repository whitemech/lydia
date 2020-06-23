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

#include <cassert>
#include <lydia/basic.hpp>
#include <lydia/pl/logic.hpp>
#include <lydia/symbol.hpp>
#include <lydia/utils/compare.hpp>
#include <utility>

namespace whitemech {
namespace lydia {

class RegExp;

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
  hash_t compute_hash_() const override;
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
  bool is_canonical(const set_formulas &container_) const;
  hash_t compute_hash_() const override;
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
  bool is_canonical(const set_formulas &container_) const;
  hash_t compute_hash_() const override;
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
  bool is_canonical(const LDLfFormula &s) const;
  hash_t compute_hash_() const override;
  virtual vec_basic get_args() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  std::shared_ptr<const LDLfFormula> get_arg() const;
  std::shared_ptr<const LDLfFormula> logical_not() const override;
};

class LDLfTemporal : public LDLfFormula {
private:
  const ldlf_ptr arg_;
  const regex_ptr regex_;

public:
  LDLfTemporal(regex_ptr regex, ldlf_ptr formula)
      : regex_{std::move(regex)}, arg_{std::move(formula)} {}
  ldlf_ptr get_formula() const { return arg_; };
  regex_ptr get_regex() const { return regex_; };
  hash_t compute_hash_() const override;
};

class LDLfDiamond : public LDLfTemporal {
public:
  const static TypeID type_code_id = TypeID::t_LDLfDiamond;
  LDLfDiamond(const regex_ptr &regex, const ldlf_ptr &formula);
  bool is_canonical(const set_formulas &container_) const;
  void accept(Visitor &v) const override;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  std::shared_ptr<const LDLfFormula> logical_not() const override;
};

class LDLfBox : public LDLfTemporal {
public:
  const static TypeID type_code_id = TypeID::t_LDLfBox;
  LDLfBox(const regex_ptr &regex, const ldlf_ptr &formula);
  bool is_canonical(const set_formulas &container_) const;
  void accept(Visitor &v) const override;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
  std::shared_ptr<const LDLfFormula> logical_not() const override;
};

class RegExp : public Basic {};

class PropositionalRegExp : public RegExp {
private:
  const std::shared_ptr<const PropositionalFormula> arg_;

public:
  const static TypeID type_code_id = TypeID::t_PropositionalRegExp;
  void accept(Visitor &v) const override;
  explicit PropositionalRegExp(std::shared_ptr<const PropositionalFormula> f);
  bool is_canonical(const PropositionalFormula &f) const;
  hash_t compute_hash_() const override;
  std::shared_ptr<const PropositionalFormula> get_arg() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
};

class TestRegExp : public RegExp {
private:
  const std::shared_ptr<const LDLfFormula> arg_;

public:
  const static TypeID type_code_id = TypeID::t_TestRegExp;
  void accept(Visitor &v) const override;
  explicit TestRegExp(std::shared_ptr<const LDLfFormula> f);
  bool is_canonical(const LDLfFormula &f) const;
  hash_t compute_hash_() const override;
  std::shared_ptr<const LDLfFormula> get_arg() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
};

class UnionRegExp : public RegExp {
private:
  const set_regex container_;

public:
  const static TypeID type_code_id = TypeID::t_UnionRegExp;
  explicit UnionRegExp(const set_regex &args);
  void accept(Visitor &v) const override;
  bool is_canonical(const set_regex &args) const;
  hash_t compute_hash_() const override;
  const set_regex &get_container() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
};

class SequenceRegExp : public RegExp {
private:
  const vec_regex container_;

public:
  const static TypeID type_code_id = TypeID::t_SequenceRegExp;
  explicit SequenceRegExp(const vec_regex &args);
  void accept(Visitor &v) const override;
  bool is_canonical(const set_regex &args) const;
  hash_t compute_hash_() const override;
  const vec_regex &get_container() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
};

class StarRegExp : public RegExp {
private:
  const regex_ptr arg_;

public:
  const static TypeID type_code_id = TypeID::t_StarRegExp;
  explicit StarRegExp(regex_ptr arg);
  void accept(Visitor &v) const override;
  bool is_canonical(const set_regex &args) const;
  hash_t compute_hash_() const override;
  const regex_ptr &get_arg() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
};

/*
 * Auxiliary construct for the delta function.
 */
class LDLfF : public LDLfFormula {
private:
  const LDLfFormula &arg_;

protected:
public:
  const static TypeID type_code_id = TypeID::t_LDLfF;
  explicit LDLfF(const LDLfFormula &formula);
  void accept(Visitor &v) const override;
  bool is_canonical(const set_regex &args) const;
  hash_t compute_hash_() const override;
  const LDLfFormula &get_arg() const;
  int compare(const Basic &rhs) const override;
  bool is_equal(const Basic &rhs) const override;
  ldlf_ptr logical_not() const override;
};

/*
 * Auxiliary construct for the delta function.
 */
class LDLfT : public LDLfFormula {
private:
  const LDLfFormula &arg_;

protected:
public:
  const static TypeID type_code_id = TypeID::t_LDLfT;
  explicit LDLfT(const LDLfFormula &formula);
  void accept(Visitor &v) const override;
  bool is_canonical(const set_regex &args) const;
  hash_t compute_hash_() const override;
  const LDLfFormula &get_arg() const;
  int compare(const Basic &rhs) const override;
  bool is_equal(const Basic &rhs) const override;
  ldlf_ptr logical_not() const override;
};

class QuotedFormula : public Basic {
private:
protected:
public:
  const static TypeID type_code_id = TypeID::t_QuotedFormula;
  const std::shared_ptr<const Basic> formula;

  /*!
   * Quote an LDLf formula. We assume it is in NNF.
   * @param f: the LDLf formula.
   */
  explicit QuotedFormula(basic_ptr formula);

  void accept(Visitor &v) const override;
  hash_t compute_hash_() const override;
  int compare(const Basic &rhs) const override;
  bool is_equal(const Basic &rhs) const override;
};

std::shared_ptr<const QuotedFormula> quote(const basic_ptr &p);

} // namespace lydia
} // namespace whitemech
