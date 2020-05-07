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

#include <lydia/pl/cnf.hpp>
#include <lydia/pl/models.hpp>
#include <lydia/to_dfa/delta_symbolic.hpp>
#include <lydia/to_dfa/nfa_state.hpp>

namespace whitemech {
namespace lydia {

NFAState::NFAState(set_formulas formulas) : formulas{std::move(formulas)} {
  this->type_code_ = type_code_id;
};

bool NFAState::is_equal(const Basic &rhs) const {
  return is_a<NFAState>(rhs) and
         unified_eq(this->formulas,
                    dynamic_cast<const NFAState &>(rhs).formulas);
}

int NFAState::compare(const Basic &rhs) const {
  assert(is_a<NFAState>(rhs));
  return unified_compare(this->formulas,
                         dynamic_cast<const NFAState &>(rhs).formulas);
}

hash_t NFAState::compute_hash_() const {
  hash_t seed = type_code_id;
  for (const auto &a : formulas)
    hash_combine<Basic>(seed, *a);
  return seed;
}

bool NFAState::is_final() const {
  // This will be put in conjunction with other formulas
  vec_prop_formulas args{std::make_shared<PropositionalTrue>(),
                         std::make_shared<PropositionalTrue>()};
  for (const auto &formula : formulas) {
    args.push_back(delta(*formula));
  }
  auto conjunction =
      PropositionalAnd(set_prop_formulas(args.begin(), args.end()));

  // to evaluate this formula, we just use an empty prop. interpretation.
  // this is because the delta with epsilon = true returns either true or false.
  auto empty = set_atoms_ptr();
  return eval(conjunction, empty);
}

set_nfa_states NFAState::next_states(const set_atoms_ptr &i) const {
  // This will be put in conjunction with other formulas
  vec_prop_formulas args{std::make_shared<PropositionalTrue>(),
                         std::make_shared<PropositionalTrue>()};
  for (const auto &formula : formulas) {
    args.push_back(delta(*formula, i));
  }
  auto conjunction =
      PropositionalAnd(set_prop_formulas(args.begin(), args.end()));

  set_nfa_states result;
  auto models = minimal_models(conjunction);
  for (const auto &model : models) {
    set_formulas tmp;
    for (const auto &atom : model) {
      assert(is_a<QuotedFormula>(*atom->symbol));
      tmp.insert(dynamic_cast<const QuotedFormula &>(*atom->symbol).formula);
    }
    result.emplace(std::make_shared<NFAState>(tmp));
  }

  return result;
}

std::vector<std::pair<set_atoms_ptr, set_nfa_states>>
NFAState::next_transitions() const {
  std::vector<std::pair<set_atoms_ptr, set_nfa_states>> result;
  std::map<set_atoms_ptr, set_nfa_states> symbol2nfastates;
  set_prop_formulas setPropFormulas;
  set_nfa_states v;
  for (const auto &f : formulas) {
    const auto &delta_formula = delta_symbolic(*f, false);
    setPropFormulas.insert(delta_formula);
  }
  auto and_ = to_cnf(*logical_and(setPropFormulas));
  const auto &all_minimal_models = all_models(*and_);
  for (const auto &model : all_minimal_models) {
    set_nfa_states nfa_states;
    set_formulas quoted_formulas;
    set_atoms_ptr symbol;
    for (const atom_ptr &ptr : model) {
      if (is_a<QuotedFormula>(*ptr->symbol))
        quoted_formulas.insert(
            dynamic_cast<const QuotedFormula &>(*ptr->symbol).formula);
      else
        symbol.insert(ptr);
    }

    auto successor = symbol2nfastates.find(symbol);
    if (successor == symbol2nfastates.end()) {
      symbol2nfastates[symbol] = set_nfa_states();
    }
    symbol2nfastates[symbol].insert(
        std::make_shared<NFAState>(quoted_formulas));
  }

  result.reserve(symbol2nfastates.size());
  for (const auto &pair : symbol2nfastates) {
    result.emplace_back(pair);
  }
  return result;
}

} // namespace lydia
} // namespace whitemech