#pragma once
/*
 * This file is part of Nike.
 *
 * Nike is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nike is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nike.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <cstdint>
#include <memory>

#include <lydia/visitor.hpp>

namespace whitemech {
namespace lydia {

/*
 * Apply a negation.
 * - in case of boolean or temporal operator, apply the
 *   duality of negation to push a negation down;
 * - in case of atomic formula, return the negation of it
 */
class NegationTransformer : public Visitor {
public:
  ltlf_ptr result;
  void visit(const LTLfTrue&) override;
  void visit(const LTLfFalse&) override;
  void visit(const LTLfAtom&) override;
  void visit(const LTLfNot&) override;
  void visit(const LTLfAnd&) override;
  void visit(const LTLfOr&) override;
  void visit(const LTLfNext&) override;
  void visit(const LTLfWeakNext&) override;
  void visit(const LTLfUntil&) override;
  void visit(const LTLfRelease&) override;
  void visit(const LTLfEventually&) override;
  void visit(const LTLfAlways&) override;

  ltlf_ptr apply(const LTLfFormula& f);
};

ltlf_ptr apply_negation(const LTLfFormula& f);

template <typename Function1, typename Function2, typename BinaryOp>
inline ltlf_ptr forward_call_to_arguments(const BinaryOp& formula,
                                          Function1 mapping_function,
                                          Function2 factory_function) {
  const auto& container = formula.get_args();
  auto new_container = vec_ltlf_formulas(container.size());
  std::transform(container.begin(), container.end(), new_container.begin(),
                 mapping_function);
  return factory_function(new_container);
}

} // namespace lydia
} // namespace whitemech