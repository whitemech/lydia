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

#include <lydia/logic/pl/cnf.hpp>

namespace whitemech::lydia {

void CNFTransformer::visit(const PropositionalTrue& f) {
  result = f.ctx().makeTrue();
}

void CNFTransformer::visit(const PropositionalFalse& f) {
  result = f.ctx().makeFalse();
}

void CNFTransformer::visit(const PropositionalAtom& f) {
  result =
      std::static_pointer_cast<const PropositionalAtom>(f.shared_from_this());
}

void CNFTransformer::visit(const PropositionalAnd& f) {
  set_prop_formulas args;
  for (const auto& subformula : f.get_container()) {
    auto subformula_cnf = apply(*subformula);
    if (is_a<PropositionalAnd>(*subformula_cnf)) {
      const auto& to_insert =
          dynamic_cast<const PropositionalAnd&>(*subformula_cnf);
      const auto& container = to_insert.get_container();
      args.insert(container.begin(), container.end());
    } else {
      args.insert(subformula_cnf);
    }
  }
  result = f.ctx().makePropAnd(args);
}

void CNFTransformer::visit(const PropositionalOr& f) {
  set_prop_formulas args;
  auto container = f.get_container();
  auto first = apply(**container.begin());
  container.erase(container.begin());
  prop_ptr tail;
  set_prop_formulas first_container;
  set_prop_formulas tail_container;

  if (container.size() == 1)
    tail = apply(**container.begin());
  else
    tail = apply(*f.ctx().makePropOr(container));

  // both first and tail are now CNF formulas.
  auto is_first_and = is_a<PropositionalAnd>(*first);
  auto is_tail_and = is_a<PropositionalAnd>(*tail);
  if (!is_first_and && !is_tail_and) {
    // lucky day! both subformulas have only one clause.
    result = f.ctx().makePropOr({first, tail});
    return;
  } else {
    // one of them is an And.
    first_container = to_container(first);
    tail_container = to_container(tail);
  }

  for (const auto& x : first_container) {
    for (const auto& y : tail_container) {
      args.insert(f.ctx().makePropOr(set_prop_formulas{x, y}));
    }
  }

  result = f.ctx().makePropAnd(args);
}

void CNFTransformer::visit(const PropositionalNot& f) {
  auto& arg = *f.get_arg();
  if (is_a<PropositionalAtom>(arg) or is_a<PropositionalTrue>(arg) or
      is_a<PropositionalFalse>(arg)) {
    result = arg.logical_not();
  } else
    result = apply(*f.get_arg()->logical_not());
}

set_prop_formulas to_container(prop_ptr p) {
  if (is_a<PropositionalAnd>(*p)) {
    return dynamic_cast<const PropositionalAnd&>(*p).get_container();
  } else if (is_a<PropositionalOr>(*p)) {
    return dynamic_cast<const PropositionalOr&>(*p).get_container();
  } else {
    return set_prop_formulas({prop_ptr(p)});
  }
}

prop_ptr CNFTransformer::apply(const PropositionalFormula& b) {
  b.accept(*this);
  return result;
}
prop_ptr to_cnf(const PropositionalFormula& f) {
  auto visitor = CNFTransformer();
  return visitor.apply(f);
}

} // namespace whitemech::lydia