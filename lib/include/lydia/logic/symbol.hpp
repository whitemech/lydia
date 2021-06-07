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

namespace whitemech::lydia {

class Symbol : public Basic {
private:
  //! name of Symbol
  std::string name_;

public:
  const static TypeID type_code_id = TypeID::t_Symbol;
  void accept(Visitor& v) const override;

  //! Symbol Constructor
  explicit Symbol(std::string name);

  inline const std::string& get_name() const { return name_; }

  //! \return Size of the hash
  hash_t compute_hash_() const override;

  /*! Equality comparator
   * \param o - Object to be compared with
   * \return whether the two objects are equal
   * */
  bool is_equal(const Basic& o) const override;

  /*! Comparison operator
   * \param o - Object to be compared with
   * \return `0` if equal, `-1` , `1` according to string compare
   * */
  int compare_(const Basic& o) const override;
};

//! inline version to return `Symbol`
inline Symbol symbol(const std::string& name) { return Symbol(name); }
} // namespace whitemech::lydia
