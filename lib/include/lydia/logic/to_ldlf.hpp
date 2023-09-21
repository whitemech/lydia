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

#include <lydia/logic/ldlf/base.hpp>
#include <lydia/logic/ltlf/base.hpp>
#include <lydia/logic/pl/base.hpp>
#include <lydia/visitor.hpp>

namespace whitemech::lydia {

class LTLfToLDLfTransformer : public Visitor {
private:
protected:
  ldlf_ptr result;

public:
  // callbacks for LDLf
  virtual void visit(const LTLfTrue&) override;
  virtual void visit(const LTLfFalse&) override;
  virtual void visit(const LTLfAtom&) override;
  virtual void visit(const LTLfAnd&) override;
  virtual void visit(const LTLfOr&) override;
  virtual void visit(const LTLfNot&) override;
  virtual void visit(const LTLfNext&) override;
  virtual void visit(const LTLfWeakNext&) override;
  virtual void visit(const LTLfUntil&) override;
  virtual void visit(const LTLfRelease&) override;
  virtual void visit(const LTLfEventually&) override;
  virtual void visit(const LTLfAlways&) override;

  ldlf_ptr apply(const LTLfFormula& b);
};

ldlf_ptr to_ldlf(const LTLfFormula& x);

} // namespace whitemech::lydia