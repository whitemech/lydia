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

#include "cuddObj.hh"
#include <cstdint>
#include <exception>
#include <memory>
#include <set>
#include <vector>

namespace whitemech {
namespace lydia {

class Basic;
class Symbol;
class LDLfFormula;
class PropositionalFormula;
class PropositionalAtom;
class NFAState;
class DFAState;
class DFATransition;

struct SharedComparator {
  template <typename T>
  bool operator()(const std::shared_ptr<T> &lhs,
                  const std::shared_ptr<T> &rhs) const {
    return (*lhs) < (*rhs);
  }
};

typedef uint64_t hash_t;
typedef std::vector<int> interpretation;
typedef std::vector<interpretation> trace;
typedef std::shared_ptr<const Basic> basic_ptr;
typedef std::shared_ptr<const PropositionalAtom> atom_ptr;
typedef std::set<atom_ptr, SharedComparator> set_atoms_ptr;
typedef std::tuple<const DFAState &, const set_atoms_ptr &, const DFAState &>
    tuple_dfa_transition;
typedef std::vector<std::shared_ptr<const Basic>> vec_basic;
typedef std::set<std::shared_ptr<const Basic>, SharedComparator> set_basic;
typedef std::vector<std::shared_ptr<const LDLfFormula>> vec_formulas;
typedef std::set<std::shared_ptr<const LDLfFormula>, SharedComparator>
    set_formulas;
typedef std::vector<std::shared_ptr<const PropositionalFormula>>
    vec_prop_formulas;
typedef std::set<std::shared_ptr<const PropositionalFormula>, SharedComparator>
    set_prop_formulas;
typedef std::shared_ptr<const NFAState> nfa_state_ptr;
typedef std::shared_ptr<const DFAState> dfa_state_ptr;
typedef std::shared_ptr<tuple_dfa_transition> dfa_transition_ptr;
typedef std::set<nfa_state_ptr, SharedComparator> set_nfa_states;
typedef std::set<dfa_state_ptr, SharedComparator> set_dfa_states;
typedef std::set<tuple_dfa_transition> set_dfa_transitions;
typedef std::vector<int> item;
typedef std::vector<CUDD::BDD> vec_bdd;

class not_implemented_error : public std::logic_error {
public:
  not_implemented_error() : std::logic_error("Function not yet implemented"){};
};

} // namespace lydia
}; // namespace whitemech
