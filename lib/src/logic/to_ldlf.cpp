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

#include <lydia/logic/to_ldlf.hpp>

namespace whitemech::lydia {

void LTLfToLDLfTransformer::visit(const LTLfTrue& f) {
  result = f.ctx().makeLdlfTrue();
}
void LTLfToLDLfTransformer::visit(const LTLfFalse& f) {
  result = f.ctx().makeLdlfFalse();
}
void LTLfToLDLfTransformer::visit(const LTLfAtom& f) {
  auto prop_atom = f.ctx().makePropAtom(f.symbol);
  auto prop_regex = f.ctx().makePropRegex(prop_atom);
  auto tt = f.ctx().makeLdlfTrue();
  result = f.ctx().makeLdlfDiamond(prop_regex, tt);
}
void LTLfToLDLfTransformer::visit(const LTLfAnd& f) {
  vec_ldlf_formulas args;
  args.reserve(f.get_args().size());
  for (const auto& subf : f.get_args()) {
    auto subf_result = apply(*subf);
    args.push_back(subf_result);
  }
  set_formulas set_args{args.begin(), args.end()};
  result = f.ctx().makeLdlfAnd(set_args);
}
void LTLfToLDLfTransformer::visit(const LTLfOr& f) {
  vec_ldlf_formulas args;
  args.reserve(f.get_args().size());
  for (const auto& subf : f.get_args()) {
    auto subf_result = apply(*subf);
    args.push_back(subf_result);
  }
  set_formulas set_args{args.begin(), args.end()};
  result = f.ctx().makeLdlfOr(set_args);
}
void LTLfToLDLfTransformer::visit(const LTLfNot& f) {
  auto subf_result = apply(*f.get_arg());
  result = f.ctx().makeLdlfNot(subf_result);
}
void LTLfToLDLfTransformer::visit(const LTLfNext& f) {
  auto subf_result = apply(*f.get_arg());
  auto prop_true = f.ctx().makeTrue();
  auto prop_regex = f.ctx().makePropRegex(prop_true);
  auto not_end = f.ctx().makeLdlfNotEnd();
  auto tail = f.ctx().makeLdlfAnd({subf_result, not_end});
  result = f.ctx().makeLdlfDiamond(prop_regex, tail);
}
void LTLfToLDLfTransformer::visit(const LTLfWeakNext& f) {
  auto subf_result = apply(*f.get_arg());
  auto prop_true = f.ctx().makeTrue();
  auto prop_regex = f.ctx().makePropRegex(prop_true);
  auto end = f.ctx().makeLdlfEnd();
  auto tail = f.ctx().makeLdlfOr({subf_result, end});
  result = f.ctx().makeLdlfBox(prop_regex, tail);
}
void LTLfToLDLfTransformer::visit(const LTLfUntil& f) {
  auto not_end = f.ctx().makeLdlfNotEnd();
  auto head_result = apply(*f.head());
  auto tail_result = apply(*f.tail());
  auto tail_and_not_end = f.ctx().makeLdlfAnd({tail_result, not_end});

  auto true_regex = f.ctx().makePropRegex(f.ctx().makeTrue());
  auto formula_test = f.ctx().makeTestRegex(head_result);
  auto seq_star =
      f.ctx().makeStarRegex(f.ctx().makeSeqRegex({formula_test, true_regex}));

  result = f.ctx().makeLdlfDiamond(seq_star, tail_and_not_end);
}
void LTLfToLDLfTransformer::visit(const LTLfRelease& f) {
  auto end = f.ctx().makeLdlfEnd();
  auto head_result = apply(*f.head());
  auto tail_result = apply(*f.tail());
  auto tail_or_end = f.ctx().makeLdlfOr({tail_result, end});
  auto true_regex = f.ctx().makePropRegex(f.ctx().makeTrue());
  auto formula_test = f.ctx().makeTestRegex(f.ctx().makeLdlfNot(head_result));
  auto seq_star =
      f.ctx().makeStarRegex(f.ctx().makeSeqRegex({formula_test, true_regex}));
  result = f.ctx().makeLdlfBox(seq_star, tail_or_end);
}

void LTLfToLDLfTransformer::visit(const LTLfEventually& f) {
  auto not_end = f.ctx().makeLdlfNotEnd();
  auto arg_result = apply(*f.get_arg());
  auto formula_and_not_end = f.ctx().makeLdlfAnd({arg_result, not_end});
  auto true_star =
      f.ctx().makeStarRegex(f.ctx().makePropRegex(f.ctx().makeTrue()));
  result = f.ctx().makeLdlfDiamond(true_star, formula_and_not_end);
}
void LTLfToLDLfTransformer::visit(const LTLfAlways& f) {
  auto end = f.ctx().makeLdlfEnd();
  auto arg_result = apply(*f.get_arg());
  auto formula_or_end = f.ctx().makeLdlfOr({arg_result, end});
  auto true_star =
      f.ctx().makeStarRegex(f.ctx().makePropRegex(f.ctx().makeTrue()));
  result = f.ctx().makeLdlfBox(true_star, formula_or_end);
}

ldlf_ptr LTLfToLDLfTransformer::apply(const LTLfFormula& b) {
  b.accept(*this);
  return result;
}

ldlf_ptr to_ldlf(const LTLfFormula& x) {
  LTLfToLDLfTransformer visitor;
  auto result = visitor.apply(x);
  return result;
}

} // namespace whitemech::lydia