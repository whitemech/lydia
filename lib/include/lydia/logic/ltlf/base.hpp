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

#include <cassert>
#include <lydia/basic.hpp>
#include <lydia/logic/pl/base.hpp>
#include <lydia/logic/symbol.hpp>
#include <lydia/utils/compare.hpp>
#include <utility>

namespace whitemech::lydia {

class RegExp;

class LTLfFormula : public Ast {
public:
  explicit LTLfFormula(AstManager& c) : Ast(c) {}
  virtual ltlf_ptr logical_not() const = 0;
};

class LTLfTrue : public LTLfFormula {
public:
  const static TypeID type_code_id = TypeID::t_LTLfTrue;
  explicit LTLfTrue(AstManager& c) : LTLfFormula(c) {
    type_code_ = type_code_id;
  }
  void accept(Visitor& v) const override;
  hash_t compute_hash_() const override;
  virtual vec_ltlf_formulas get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  ltlf_ptr logical_not() const override;
};

class LTLfFalse : public LTLfFormula {
public:
  const static TypeID type_code_id = TypeID::t_LTLfFalse;
  explicit LTLfFalse(AstManager& c) : LTLfFormula(c) {
    type_code_ = type_code_id;
  }
  void accept(Visitor& v) const override;
  hash_t compute_hash_() const override;
  virtual vec_ltlf_formulas get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  ltlf_ptr logical_not() const override;
};

class LTLfAtom : public LTLfFormula {
public:
  const static TypeID type_code_id = TypeID::t_LTLfAtom;
  const symbol_ptr symbol;
  explicit LTLfAtom(AstManager& c, const std::string&);
  explicit LTLfAtom(AstManager& c, const symbol_ptr& p);
  void accept(Visitor& v) const override;
  hash_t compute_hash_() const override;
  int compare_(const Basic& rhs) const override;
  bool is_equal(const Basic& rhs) const override;
  ltlf_ptr logical_not() const override;
  set_ltlf_formulas get_container() const {
    return set_ltlf_formulas{
        std::static_pointer_cast<const LTLfAtom>(this->shared_from_this())};
  };
};

class LTLfAnd : public LTLfFormula {
private:
  const set_ltlf_formulas container_;

public:
  const static TypeID type_code_id = TypeID::t_LTLfAnd;
  void accept(Visitor& v) const override;
  explicit LTLfAnd(AstManager& c, const set_ltlf_formulas& s);
  bool is_canonical(const set_ltlf_formulas& container_) const;
  hash_t compute_hash_() const override;
  virtual vec_ltlf_formulas get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  const set_ltlf_formulas& get_container() const;
  ltlf_ptr logical_not() const override;
};

class LTLfOr : public LTLfFormula {
private:
  const set_ltlf_formulas container_;

public:
  const static TypeID type_code_id = TypeID::t_LTLfOr;
  void accept(Visitor& v) const override;
  explicit LTLfOr(AstManager& c, const set_ltlf_formulas& s);
  bool is_canonical(const set_ltlf_formulas& container_) const;
  hash_t compute_hash_() const override;
  virtual vec_ltlf_formulas get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  const set_ltlf_formulas& get_container() const;
  ltlf_ptr logical_not() const override;
};

class LTLfNot : public LTLfFormula {
private:
  const ltlf_ptr arg_;

public:
  const static TypeID type_code_id = TypeID::t_LTLfNot;
  void accept(Visitor& v) const override;
  explicit LTLfNot(AstManager& c, const ltlf_ptr& in);
  bool is_canonical(const LTLfFormula& s) const;
  hash_t compute_hash_() const override;
  virtual vec_basic get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  ltlf_ptr get_arg() const;
  ltlf_ptr logical_not() const override;
};

class LTLfNext : public LTLfFormula {
private:
  const ltlf_ptr arg_;

public:
  const static TypeID type_code_id = TypeID::t_LTLfNext;
  void accept(Visitor& v) const override;
  explicit LTLfNext(AstManager& c, const ltlf_ptr& in);
  bool is_canonical(const LTLfFormula& s) const;
  hash_t compute_hash_() const override;
  virtual vec_basic get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  ltlf_ptr get_arg() const;
  ltlf_ptr logical_not() const override;
};

class LTLfWeakNext : public LTLfFormula {
private:
  const ltlf_ptr arg_;

public:
  const static TypeID type_code_id = TypeID::t_LTLfWeakNext;
  void accept(Visitor& v) const override;
  explicit LTLfWeakNext(AstManager& c, const ltlf_ptr& in);
  bool is_canonical(const LTLfFormula& s) const;
  hash_t compute_hash_() const override;
  virtual vec_basic get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  ltlf_ptr get_arg() const;
  ltlf_ptr logical_not() const override;
};

class LTLfUntil : public LTLfFormula {
private:
  const ltlf_ptr arg_1_;
  const ltlf_ptr arg_2_;
  const set_ltlf_formulas container_;

public:
  const static TypeID type_code_id = TypeID::t_LTLfUntil;
  void accept(Visitor& v) const override;
  explicit LTLfUntil(AstManager& c, const ltlf_ptr& arg_1,
                     const ltlf_ptr& arg_2);
  bool is_canonical(const set_ltlf_formulas& container_) const;
  hash_t compute_hash_() const override;
  virtual vec_ltlf_formulas get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  const set_ltlf_formulas& get_container() const;
  ltlf_ptr logical_not() const override;
};

class LTLfRelease : public LTLfFormula {
private:
  const ltlf_ptr arg_1_;
  const ltlf_ptr arg_2_;
  const set_ltlf_formulas container_;

public:
  const static TypeID type_code_id = TypeID::t_LTLfRelease;
  void accept(Visitor& v) const override;
  explicit LTLfRelease(AstManager& c, const ltlf_ptr& arg_1,
                       const ltlf_ptr& arg_2);
  bool is_canonical(const set_ltlf_formulas& container_) const;
  hash_t compute_hash_() const override;
  virtual vec_ltlf_formulas get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  const set_ltlf_formulas& get_container() const;
  ltlf_ptr logical_not() const override;
};

class LTLfEventually : public LTLfFormula {
private:
  const ltlf_ptr arg_;

public:
  const static TypeID type_code_id = TypeID::t_LTLfEventually;
  void accept(Visitor& v) const override;
  explicit LTLfEventually(AstManager& c, const ltlf_ptr& in);
  bool is_canonical(const LTLfFormula& s) const;
  hash_t compute_hash_() const override;
  virtual vec_basic get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  ltlf_ptr get_arg() const;
  ltlf_ptr logical_not() const override;
};

class LTLfAlways : public LTLfFormula {
private:
  const ltlf_ptr arg_;

public:
  const static TypeID type_code_id = TypeID::t_LTLfAlways;
  void accept(Visitor& v) const override;
  explicit LTLfAlways(AstManager& c, const ltlf_ptr& in);
  bool is_canonical(const LTLfFormula& s) const;
  hash_t compute_hash_() const override;
  virtual vec_basic get_args() const;
  bool is_equal(const Basic& o) const override;
  int compare_(const Basic& o) const override;
  ltlf_ptr get_arg() const;
  ltlf_ptr logical_not() const override;
};

} // namespace whitemech::lydia
