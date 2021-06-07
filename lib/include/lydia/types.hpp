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

#include <cstdint>
#include <cuddObj.hh>
#include <exception>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace whitemech::lydia {

class Basic;
class Symbol;
class LTLfFormula;
class LTLfTrue;
class LTLfFalse;
class LTLfAtom;
class LTLfAnd;
class LTLfOr;
class LTLfNot;
class LTLfNext;
class LTLfWeakNext;
class LTLfUntil;
class LTLfRelease;
class LTLfEventually;
class LTLfAlways;
class LDLfFormula;
class LDLfTrue;
class LDLfFalse;
class LDLfAnd;
class LDLfOr;
class LDLfNot;
class RegExp;
class PropositionalRegExp;
class TestRegExp;
class UnionRegExp;
class SequenceRegExp;
class StarRegExp;
class LDLfTemporal;
class LDLfDiamond;
class LDLfBox;
class PropositionalFormula;
class PropositionalAtom;
class PropositionalTrue;
class PropositionalFalse;
class PropositionalAtom;
class QuotedFormula;
class LDLfF;
class LDLfT;
class LDLfQ;
class PropositionalAnd;
class PropositionalOr;
class PropositionalNot;
class NFAState;
class DFAState;
class DFATransition;
class dfa;
class abstract_dfa;
class BDDStrategy;

struct SharedComparator {
  template <typename T>
  bool operator()(const std::shared_ptr<T>& lhs,
                  const std::shared_ptr<T>& rhs) const {
    return (*lhs) < (*rhs);
  }
};

typedef uint64_t hash_t;
// a list of 0-1 that denotes the value of truth of the i_th variable.
typedef std::vector<int> interpretation;
typedef std::vector<interpretation> trace;
// a set of indexes whose corresponding variables must be interpreted as true
typedef std::set<int> interpretation_set;
typedef std::map<int, bool> interpretation_map;
typedef std::shared_ptr<const Basic> basic_ptr;
typedef std::shared_ptr<const PropositionalAtom> atom_ptr;
typedef std::set<atom_ptr, SharedComparator> set_atoms_ptr;
typedef std::map<atom_ptr, int, SharedComparator> map_atoms_ptr;
typedef std::tuple<const DFAState&, const set_atoms_ptr&, const DFAState&>
    tuple_dfa_transition;
typedef std::vector<std::shared_ptr<const Basic>> vec_basic;
typedef std::set<std::shared_ptr<const Basic>, SharedComparator> set_basic;
typedef std::shared_ptr<const LDLfFormula> ldlf_ptr;
typedef std::shared_ptr<const LTLfFormula> ltlf_ptr;
typedef std::vector<std::shared_ptr<const LTLfFormula>> vec_ltlf_formulas;
typedef std::set<std::shared_ptr<const LTLfFormula>, SharedComparator>
    set_ltlf_formulas;
typedef std::vector<std::shared_ptr<const LDLfFormula>> vec_formulas;
typedef std::set<std::shared_ptr<const LDLfFormula>, SharedComparator>
    set_formulas;
typedef std::shared_ptr<const PropositionalFormula> prop_ptr;
typedef std::shared_ptr<const Symbol> symbol_ptr;
typedef std::vector<prop_ptr> vec_prop_formulas;
typedef std::set<prop_ptr, SharedComparator> set_prop_formulas;
typedef std::shared_ptr<const RegExp> regex_ptr;
typedef std::set<regex_ptr, SharedComparator> set_regex;
typedef std::vector<regex_ptr> vec_regex;
typedef std::shared_ptr<const NFAState> nfa_state_ptr;
typedef std::shared_ptr<const DFAState> dfa_state_ptr;
typedef std::shared_ptr<tuple_dfa_transition> dfa_transition_ptr;
typedef std::set<nfa_state_ptr, SharedComparator> set_nfa_states;
typedef std::vector<nfa_state_ptr> vec_nfa_states;
typedef std::set<dfa_state_ptr, SharedComparator> set_dfa_states;
typedef std::vector<dfa_state_ptr> vec_dfa_states;
typedef std::map<dfa_state_ptr, int, SharedComparator> map_dfa_states;
typedef std::set<tuple_dfa_transition> set_dfa_transitions;
typedef std::vector<int> item;
typedef std::vector<CUDD::BDD> vec_bdd;
typedef unsigned long int state_t;
typedef unsigned long int variable_t;
typedef bool bit;
typedef std::shared_ptr<dfa> dfa_ptr;
typedef std::shared_ptr<abstract_dfa> adfa_ptr;

class not_implemented_error : public std::exception {
public:
  const char* what() const noexcept override {
    return "Function not yet implemented.";
  }
};

struct cmp_set_of_ptr {
  template <typename T, typename U>
  bool operator()(const std::set<T, U>& a, const std::set<T, U>& b) const {
    return unified_compare(a, b) < 0;
  }
};

} // namespace whitemech::lydia
