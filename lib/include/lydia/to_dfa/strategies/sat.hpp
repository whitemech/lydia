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
#include <lydia/atom_visitor.hpp>
#include <lydia/dfa.hpp>
#include <lydia/nnf.hpp>
#include <lydia/pl/logic.hpp>
#include <lydia/pl/replace.hpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/dfa_state.hpp>
#include <lydia/utils/misc.hpp>
#include <queue>
#include <utility>

namespace whitemech {
namespace lydia {

class SATStrategy : public Strategy {
private:
  const CUDD::Cudd &mgr;
  const size_t max_nb_bits;
  dfa_ptr automaton;
  map_atoms_ptr atom2index;

public:
  explicit SATStrategy(const CUDD::Cudd &mgr, uint32_t max_nb_bits = 10)
      : mgr{mgr}, max_nb_bits{max_nb_bits} {};

  dfa_ptr to_dfa(const LDLfFormula &formula);

  std::vector<std::pair<dfa_state_ptr, CUDD::BDD>>
  next_transitions(const DFAState &state);
  std::map<nfa_state_ptr, CUDD::BDD, SharedComparator>
  next_transitions(const NFAState &state);

  static std::map<nfa_state_ptr, set_prop_formulas, SharedComparator>
  next_transitions_from_delta_formula(const PropositionalFormula &f);

  void add_transition(int from_index, CUDD::BDD guard, int to_index);

  interpretation_map
  get_interpretation_from_symbol(const set_atoms_ptr &symbol,
                                 const map_atoms_ptr &atom2index);
};

class DualRailEncodingVisitor : public Visitor {
private:
  prop_ptr result;

public:
  void visit(const PropositionalTrue &);
  void visit(const PropositionalFalse &);
  void visit(const PropositionalAtom &);
  void visit(const PropositionalAnd &);
  void visit(const PropositionalOr &);
  void visit(const PropositionalNot &);

  prop_ptr apply(const PropositionalFormula &);
};

class ClauseExtractorVisitor : public Visitor {
private:
  bool in_or = false;
  bool in_not = false;
  set_atoms_ptr current_clause;

public:
  std::vector<set_atoms_ptr> result;

  void visit(const PropositionalTrue &);
  void visit(const PropositionalFalse &);
  void visit(const PropositionalAtom &);
  void visit(const PropositionalAnd &);
  void visit(const PropositionalOr &);
  void visit(const PropositionalNot &);

  void apply(const PropositionalFormula &);
};

class BDDVisitor : public Visitor {
private:
  dfa_ptr automaton;
  const map_atoms_ptr &atom2index;

public:
  CUDD::BDD result;

  BDDVisitor(dfa_ptr automaton, const map_atoms_ptr &m)
      : automaton{automaton}, atom2index{m} {}

  void visit(const PropositionalTrue &);
  void visit(const PropositionalFalse &);
  void visit(const PropositionalAtom &);
  void visit(const PropositionalAnd &);
  void visit(const PropositionalOr &);
  void visit(const PropositionalNot &);

  CUDD::BDD apply(const PropositionalFormula &);
};

/*
 * f must be a CNF formula.
 * It returns all the models of the dual-rail encoding of the formula.
 */
std::vector<set_atoms_ptr> all_prime_implicants(const PropositionalFormula &f);

} // namespace lydia
} // namespace whitemech