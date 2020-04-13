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

namespace whitemech {
namespace lydia {

class PropositionalLogicFormula : public Basic {};

class PropositionalLogicTrue : public PropositionalLogicFormula {
public:
  const static TypeID type_code_id = TypeID::t_PropositionalLogicTrue;

  void accept(Visitor &v) const override{};
  hash_t __hash__() const override {
    hash_t seed = type_code_id;
    return seed;
  }

  int compare(const Basic &rhs) const override {
    return is_a<PropositionalLogicTrue>(rhs);
  };

  bool is_equal(const Basic &rhs) const override {
    return is_a<PropositionalLogicTrue>(rhs);
  };
};

class PropositionalLogicFalse : public PropositionalLogicFormula {
public:
  const static TypeID type_code_id = TypeID::t_PropositionalLogicFalse;

  void accept(Visitor &v) const override{};
  hash_t __hash__() const override {
    hash_t seed = type_code_id;
    return seed;
  }

  int compare(const Basic &rhs) const override {
    return is_a<PropositionalLogicFalse>(rhs);
  };

  bool is_equal(const Basic &rhs) const override {
    return is_a<PropositionalLogicFalse>(rhs);
  };
};

} // namespace lydia
} // namespace whitemech