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
#include "propositional_logic.hpp"
#include "symbol.hpp"
#include "utils/compare.hpp"
#include <cassert>

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

// TODO consider template check "enable_if_t" vs static_assert in constructor
// template <class T, class=std::enable_if_t<std::is_base_of_v<RegExp, T>>>
template <class T> class LDLfTemporal : public LDLfFormula {
private:
  const ldlf_ptr arg_;
  const std::shared_ptr<const T> regex_;

public:
  explicit LDLfTemporal<T>(const std::shared_ptr<const T> &regex,
                           const ldlf_ptr &formula)
      : regex_{regex}, arg_{formula} {
    static_assert(std::is_base_of<RegExp, T>::value,
                  "concrete RegExp class not derived from RegExp");
  }
  ldlf_ptr get_formula() const { return arg_; };
  std::shared_ptr<const T> get_regex() const { return regex_; };
  hash_t compute_hash_() const override {
    hash_t seed = this->get_type_code();
    hash_combine<Basic>(seed, *this->get_regex());
    hash_combine<Basic>(seed, *this->get_formula());
    return seed;
  };
};

template <class T> class LDLfDiamond : public LDLfTemporal<T> {
public:
  const static TypeID type_code_id = TypeID::t_LDLfDiamond;
  bool is_canonical(const set_formulas &container_) const;
  void accept(Visitor &v) const override { v.visit(*this); };
  explicit LDLfDiamond<T>(const std::shared_ptr<const T> &regex,
                          const ldlf_ptr &formula)
      : LDLfTemporal<T>(regex, formula) {
    this->type_code_ = type_code_id;
  }

  bool is_equal(const Basic &o) const override {
    return is_a<LDLfDiamond<T>>(o) and
           unified_eq(this->get_regex(),
                      dynamic_cast<const LDLfDiamond<T> &>(o).get_regex()) and
           unified_eq(this->get_formula(),
                      dynamic_cast<const LDLfDiamond<T> &>(o).get_formula());
  };
  int compare(const Basic &o) const override {
    auto regex_compare = unified_compare(
        this->get_regex(), dynamic_cast<const LDLfDiamond &>(o).get_regex());
    if (regex_compare != 0)
      return regex_compare;
    return unified_compare(this->get_formula(),
                           dynamic_cast<const LDLfDiamond &>(o).get_formula());
  };
  std::shared_ptr<const LDLfFormula> logical_not() const override {
    return std::make_shared<LDLfBox<T>>(this->get_regex(),
                                        this->get_formula()->logical_not());
  };
};

template <class T> class LDLfBox : public LDLfTemporal<T> {
public:
  const static TypeID type_code_id = TypeID::t_LDLfBox;
  bool is_canonical(const set_formulas &container_) const;
  void accept(Visitor &v) const override { v.visit(*this); };
  explicit LDLfBox<T>(const std::shared_ptr<const T> &regex,
                      const ldlf_ptr &formula)
      : LDLfTemporal<T>(regex, formula) {
    this->type_code_ = type_code_id;
  }

  bool is_equal(const Basic &o) const override {
    return is_a<LDLfBox<T>>(o) and
           unified_eq(this->get_regex(),
                      dynamic_cast<const LDLfBox<T> &>(o).get_regex()) and
           unified_eq(this->get_formula(),
                      dynamic_cast<const LDLfBox<T> &>(o).get_formula());
  };
  int compare(const Basic &o) const override {
    auto regex_compare = unified_compare(
        this->get_regex(), dynamic_cast<const LDLfBox &>(o).get_regex());
    if (regex_compare != 0)
      return regex_compare;
    return unified_compare(this->get_formula(),
                           dynamic_cast<const LDLfBox &>(o).get_formula());
  };
  std::shared_ptr<const LDLfFormula> logical_not() const override {
    return std::make_shared<LDLfDiamond<T>>(this->get_regex(),
                                            this->get_formula()->logical_not());
  };
};

class RegExp : public Basic {};

class PropositionalRegExp : public RegExp {
private:
  const std::shared_ptr<const PropositionalFormula> arg_;

public:
  const static TypeID type_code_id = TypeID::t_PropositionalRegExp;
  void accept(Visitor &v) const override{};
  explicit PropositionalRegExp(std::shared_ptr<const PropositionalFormula> f);
  bool is_canonical(const set_formulas &container_) const;
  hash_t compute_hash_() const override;
  std::shared_ptr<const PropositionalFormula> get_arg() const;
  bool is_equal(const Basic &o) const override;
  int compare(const Basic &o) const override;
};

class QuotedFormula : public Basic {
private:
protected:
public:
  const static TypeID type_code_id = TypeID::t_QuotedFormula;
  const std::shared_ptr<const LDLfFormula> formula;

  /*!
   * Quote an LDLf formula. We assume it is in NNF.
   * @param f: the LDLf formula.
   */
  explicit QuotedFormula(ldlf_ptr formula) : formula{std::move(formula)} {
    this->type_code_ = TypeID::t_QuotedFormula;
  }

  void accept(Visitor &v) const override;
  hash_t compute_hash_() const override;
  int compare(const Basic &rhs) const override;
  bool is_equal(const Basic &rhs) const override;
};

std::shared_ptr<const QuotedFormula> quote(const ldlf_ptr &p);

} // namespace lydia
} // namespace whitemech
