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
#include "logic.hpp"

namespace whitemech {
namespace lydia {

class Visitor {
public:
  virtual ~Visitor(){};

  virtual void visit(const Symbol &) = 0;
  virtual void visit(const LDLfBooleanAtom &) = 0;
  virtual void visit(const LDLfAnd &) = 0;
  virtual void visit(const LDLfOr &) = 0;
  virtual void visit(const LDLfNot &) = 0;
};

template <class Derived, class Base = Visitor> class BaseVisitor : public Base {

public:
  virtual void visit(const Symbol &x) {
    dynamic_cast<Derived *>(this)->bvisit(x);
  };
  virtual void visit(const LDLfBooleanAtom &x) {
    dynamic_cast<Derived *>(this)->bvisit(x);
  };
  virtual void visit(const LDLfNot &x) {
    dynamic_cast<Derived *>(this)->bvisit(x);
  };
  virtual void visit(const LDLfAnd &x) {
    dynamic_cast<Derived *>(this)->bvisit(x);
  };
  virtual void visit(const LDLfOr &x) {
    dynamic_cast<Derived *>(this)->bvisit(x);
  };
};

} // namespace lydia
} // namespace whitemech
