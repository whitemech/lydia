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
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>

namespace whitemech::lydia {

class Ast;

// This is the internal comparison functor for hash-consing AST nodes.
struct ast_eq_proc {
  bool operator()(basic_ptr const &b1, basic_ptr const &b2) const {
    return b1->hash() == b2->hash() && b1->compare(*b2) == 0;
  }
};

class AstManager {

private:
  prop_ptr prop_true_;
  prop_ptr prop_false_;
  ldlf_ptr ldlf_true_;
  ldlf_ptr ldlf_false_;
  std::unordered_set<basic_ptr, std::hash<basic_ptr>, ast_eq_proc> table;
  void init();

  template <typename T, typename = typename std::enable_if<
                            std::is_base_of<Basic, T>::value>::type>
  std::shared_ptr<T> insert_if_not_available_(const std::shared_ptr<T> &ptr) {
    auto it = table.find(ptr);
    if (it == table.end()) {
      table.insert(ptr);
      return ptr;
    } else {
      return std::static_pointer_cast<T>(*it);
    }
  }

public:
  AstManager() { init(); }

  basic_ptr makeSymbol(const std::string &name);

  prop_ptr makeTrue();
  prop_ptr makeFalse();
  atom_ptr makePropAtom(const std::string &name);
  atom_ptr makePropAtom(const basic_ptr &ptr);
  prop_ptr makePropAnd(const set_prop_formulas &args);
  prop_ptr makePropOr(const set_prop_formulas &args);
  prop_ptr makePropNot(const prop_ptr &arg);

  ldlf_ptr makeLdlfTrue();
  ldlf_ptr makeLdlfFalse();
  ldlf_ptr makeLdlfAnd(const set_formulas &args);
  ldlf_ptr makeLdlfOr(const set_formulas &args);
  ldlf_ptr makeLdlfNot(const ldlf_ptr &arg);
  ldlf_ptr makeLdlfBox(const regex_ptr &arg_r, const ldlf_ptr &arg);
  ldlf_ptr makeLdlfDiamond(const regex_ptr &arg_r, const ldlf_ptr &arg_f);
  ldlf_ptr makeLdlfT(const ldlf_ptr &arg);
  ldlf_ptr makeLdlfF(const ldlf_ptr &arg);

  regex_ptr makePropRegex(const prop_ptr &ptr);
  regex_ptr makeSeqRegex(const vec_regex &ptr);
  regex_ptr makeUnionRegex(const set_regex &ptr);
  regex_ptr makeStarRegex(const regex_ptr &ptr);
  regex_ptr makeTestRegex(const ldlf_ptr &ptr);
};

class Ast : public Basic {
protected:
  AstManager *m_ctx;

public:
  explicit Ast(AstManager &c) : m_ctx(&c) {}
  Ast(const Ast &s) : m_ctx(s.m_ctx) {}
  AstManager &ctx() const { return *m_ctx; }
  friend void check_context(Ast const &a, Ast const &b) {
    assert(a.m_ctx == b.m_ctx);
  };
};

// TODO remove
static AstManager context = AstManager{};

} // namespace whitemech::lydia