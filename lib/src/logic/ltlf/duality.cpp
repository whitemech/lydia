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

#include <lydia/logic/ltlf/duality.hpp>
#include <lydia/logic/nnf.hpp>

namespace whitemech {
namespace lydia {

void NegationTransformer::visit(const LTLfTrue& formula) {
  result = formula.ctx().makeLtlfFalse();
}

void NegationTransformer::visit(const LTLfFalse& formula) {
  result = formula.ctx().makeLtlfTrue();
}

void NegationTransformer::visit(const LTLfAtom& formula) {
  //  nnf(~a) = !a | end
  auto& context = formula.ctx();
  auto atom = context.makeLtlfAtom(formula.symbol);
  auto not_atom = context.makeLtlfNot(atom);
  auto end = context.makeLtlfEnd();

  result = context.makeLtlfOr(set_ltlf_formulas{not_atom, end});
}

void NegationTransformer::visit(const LTLfNot& formula) {
  if (is_a<LTLfAtom>(*formula.get_arg())) {
    // keep the not if arg is an atom
    auto atom = std::static_pointer_cast<const LTLfAtom>(formula.get_arg());
    result = formula.ctx().makeLtlfAtom(atom->symbol);
  } else {
    // nnf(~~f) = nnf(f)
    result = to_nnf(*formula.get_arg());
  }
}

void NegationTransformer::visit(const LTLfAnd& formula) {
  result = forward_call_to_arguments(
      formula, [this](const ltlf_ptr& formula) { return apply(*formula); },
      [formula](const vec_ltlf_formulas& container) {
        set_ltlf_formulas set(container.begin(), container.end());
        return formula.ctx().makeLtlfOr(set);
      });
}

void NegationTransformer::visit(const LTLfOr& formula) {
  result = forward_call_to_arguments(
      formula, [this](const ltlf_ptr& formula) { return apply(*formula); },
      [formula](const vec_ltlf_formulas& container) {
        set_ltlf_formulas set(container.begin(), container.end());
        return formula.ctx().makeLtlfAnd(set);
      });
}

void NegationTransformer::visit(const LTLfNext& formula) {
  result = formula.ctx().makeLtlfWeakNext(apply(*formula.get_arg()));
}

void NegationTransformer::visit(const LTLfWeakNext& formula) {
  result = formula.ctx().makeLtlfNext(apply(*formula.get_arg()));
}

void NegationTransformer::visit(const LTLfUntil& formula) {
  result = forward_call_to_arguments(
      formula, [this](const ltlf_ptr& formula) { return apply(*formula); },
      [formula](const vec_ltlf_formulas& container) {
        assert(container.size() == 2);
        return formula.ctx().makeLtlfRelease(container[0], container[1]);
      });
}
void NegationTransformer::visit(const LTLfRelease& formula) {
  result = forward_call_to_arguments(
      formula, [this](const ltlf_ptr& formula) { return apply(*formula); },
      [formula](const vec_ltlf_formulas& container) {
        assert(container.size() == 2);
        return formula.ctx().makeLtlfUntil(container[0], container[1]);
      });
}

void NegationTransformer::visit(const LTLfEventually& formula) {
  result = formula.ctx().makeLtlfAlways(apply(*formula.get_arg()));
}

void NegationTransformer::visit(const LTLfAlways& formula) {
  result = formula.ctx().makeLtlfEventually(apply(*formula.get_arg()));
}

ltlf_ptr NegationTransformer::apply(const LTLfFormula& f) {
  f.accept(*this);
  return result;
}

ltlf_ptr apply_negation(const LTLfFormula& f) {
  auto visitor = NegationTransformer{};
  return visitor.apply(f);
}

} // namespace lydia
} // namespace whitemech