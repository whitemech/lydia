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
#include <cppitertools/powerset.hpp>
#include <lydia/pl/cnf.hpp>
#include <lydia/pl/models/base.hpp>
#include <lydia/pl/models/naive.hpp>
#include <lydia/pl/models/sat.hpp>
#include <lydia/to_dfa/delta_symbolic.hpp>
#include <lydia/to_dfa/strategies/sat.hpp>

namespace whitemech {
namespace lydia {

adfa_ptr SATStrategy::to_dfa(const LDLfFormula &formula) {
  //  build initial state of the DFA.
  auto formula_nnf = to_nnf(formula);
  set_formulas initial_state_formulas{formula_nnf};
  dfa_state_ptr initial_state =
      std::make_shared<DFAState>(initial_state_formulas);

  // find all atoms
  set_atoms_ptr atoms = find_atoms(*formula_nnf);
  int index = 0;
  for (const auto &atom : atoms)
    atom2index[atom] = index++;

  automaton = std::make_shared<dfa>(mgr, max_nb_bits, atoms.size());
  automaton->add_state();
  automaton->set_initial_state(1);

  //  Check if the initial state is final
  if (initial_state->is_final()) {
    automaton->set_final_state(1, true);
  }

  // BFS exploration of the automaton.
  map_dfa_states discovered;
  std::queue<std::pair<dfa_state_ptr, int>> to_be_visited;
  to_be_visited.push(std::make_pair(initial_state, 1));
  while (!to_be_visited.empty()) {
    auto pair = to_be_visited.front();
    to_be_visited.pop();
    const dfa_state_ptr current_state = pair.first;
    auto current_state_index = pair.second;
    vec_dfa_states next_states;
    std::vector<set_atoms_ptr> symbols;

    const auto &next_transitions = this->next_transitions(*current_state);

    for (const auto &symbol_state : next_transitions) {
      const auto &next_state = symbol_state.first;
      const auto &symbol = symbol_state.second;
      // update states/transitions
      int next_state_index = 0;
      if (discovered.find(next_state) == discovered.end()) {
        next_state_index = automaton->add_state();
        discovered[next_state] = next_state_index;
        to_be_visited.push(std::make_pair(next_state, next_state_index));
        if (next_state->is_final()) {
          automaton->set_final_state(next_state_index, true);
        }
      } else {
        next_state_index = discovered[next_state];
      }

      add_transition(current_state_index, symbol, next_state_index);
    }
  }
  return automaton;
}

interpretation_map
SATStrategy::get_interpretation_from_symbol(const set_atoms_ptr &symbol,
                                            const map_atoms_ptr &atom2index) {
  interpretation_map x;
  for (const atom_ptr &atom : symbol) {
    auto literal =
        std::static_pointer_cast<const QuotedFormula>(atom->symbol)->formula;
    bool is_not = false;
    atom_ptr variable;
    if (is_a<PropositionalNot>(*literal)) {
      is_not = true;
      variable = std::static_pointer_cast<const PropositionalAtom>(
          dynamic_cast<const PropositionalNot &>(*literal).get_arg());
    } else {
      variable = std::static_pointer_cast<const PropositionalAtom>(literal);
    }
    x[atom2index.at(variable)] = not is_not;
  }
  return x;
}

void SATStrategy::add_transition(int from_index, CUDD::BDD guard,
                                 int to_index) {
  std::string to_binary = state2bin(to_index, automaton->nb_bits, true);
  //  Update each root BDD.
  guard = guard * automaton->state2bdd(from_index);
  assert(automaton->nb_bits == to_binary.length());
  for (int i = 0; i < automaton->nb_bits; i++) {
    bool result = int(to_binary[i]) - '0';
    if (result)
      automaton->root_bdds[i] += guard;
  }
}

std::vector<std::pair<dfa_state_ptr, CUDD::BDD>>
SATStrategy::next_transitions(const DFAState &state) {
  std::vector<std::pair<dfa_state_ptr, CUDD::BDD>> result;
  std::map<set_atoms_ptr, set_nfa_states, cmp_set_of_ptr> symbol2nfastates;
  set_dfa_states discovered;
  set_nfa_states nfa_states;
  set_atoms_ptr symbol;
  std::map<nfa_state_ptr, CUDD::BDD, SharedComparator> all_transitions;
  for (const auto &nfa_state : state.states) {
    const auto &next_transitions = this->next_transitions(*nfa_state);
    for (const auto &pair : next_transitions) {
      if (all_transitions.find(pair.first) == all_transitions.end()) {
        all_transitions[pair.first] = automaton->mgr.bddZero();
      }
      all_transitions[pair.first] += pair.second;
    }
  }

  std::vector<std::pair<nfa_state_ptr, CUDD::BDD>> all_transitions_vec(
      all_transitions.begin(), all_transitions.end());
  size_t N = all_transitions_vec.size();
  if (N == 0)
    return result;
  size_t nb_combinations = pow(2, N);
  for (size_t i = 0; i < nb_combinations; i++) {
    const auto &combination = state2bin(i, N);
    set_nfa_states current_state{};
    CUDD::BDD current_label = automaton->mgr.bddOne();
    for (size_t j = 0; j < combination.size(); j++) {
      bool membership_bit = combination[j] == '1';
      const auto &state_label = all_transitions_vec[j];
      current_label *=
          (membership_bit ? state_label.second : !state_label.second);
      if (membership_bit)
        current_state.insert(state_label.first);
    }
    if (current_label.IsZero())
      continue;
    dfa_state_ptr current_dfa_state = std::make_shared<DFAState>(current_state);
    result.emplace_back(current_dfa_state, current_label);
  }
  return result;
}

std::map<nfa_state_ptr, CUDD::BDD, SharedComparator>
SATStrategy::next_transitions(const NFAState &state) {
  set_prop_formulas setPropFormulas;
  for (const auto &f : state.formulas) {
    const auto &delta_formula = delta_symbolic(*f, false);
    setPropFormulas.insert(delta_formula);
  }
  auto and_ = logical_and(setPropFormulas);
  auto transitions_by_state_map =
      this->next_transitions_from_delta_formula(*and_);

  std::map<nfa_state_ptr, CUDD::BDD, SharedComparator> result;

  auto bdd_visitor = BDDVisitor(automaton, atom2index);
  for (const auto &pair : transitions_by_state_map) {
    CUDD::BDD tmp = bdd_visitor.apply(*logical_or(pair.second));
    result[pair.first] = tmp;
  }
  return result;
}

std::map<nfa_state_ptr, set_prop_formulas, SharedComparator>
SATStrategy::next_transitions_from_delta_formula(
    const PropositionalFormula &f) {
  std::vector<std::pair<set_atoms_ptr, set_nfa_states>> next_transitions;

  std::vector<set_atoms_ptr> prime_implicants = all_prime_implicants(f);

  // group prime implicants per state
  std::map<nfa_state_ptr, set_prop_formulas, SharedComparator>
      transitions_by_state;

  // group prime implicants per label
  //  std::map<set_prop_formulas, set_nfa_states, cmp_set_of_ptr> transitions;

  for (const auto &prime_implicant : prime_implicants) {
    set_prop_formulas labels;
    set_formulas state;
    for (const auto &atom : prime_implicant) {
      auto literal = dynamic_cast<const QuotedFormula &>(*atom->symbol).formula;
      basic_ptr variable;
      bool is_not = false;
      if (is_a<PropositionalNot>(*literal)) {
        is_not = true;
        auto not_atom =
            std::static_pointer_cast<const PropositionalNot>(literal);
        variable = not_atom->get_arg();
      } else {
        variable =
            std::static_pointer_cast<const PropositionalAtom>(literal)->symbol;
      }

      if (is_a<QuotedFormula>(*variable)) {
        // pick the LDLf formula
        assert(!is_not);
        auto ldlf_formula = std::static_pointer_cast<const LDLfFormula>(
            dynamic_cast<const QuotedFormula &>(*variable).formula);
        state.insert(ldlf_formula);
      } else {
        labels.insert(
            std::static_pointer_cast<const PropositionalFormula>(literal));
      }
    }
    auto and_ = logical_and(labels);
    auto nfa_state = std::make_shared<NFAState>(state);
    if (transitions_by_state.find(nfa_state) == transitions_by_state.end()) {
      transitions_by_state[nfa_state] = set_prop_formulas{};
    }
    transitions_by_state[nfa_state].insert(and_);
  }
  return transitions_by_state;
}

void DualRailEncodingVisitor::visit(const PropositionalTrue &f) {
  result =
      std::static_pointer_cast<const PropositionalTrue>(f.shared_from_this());
}
void DualRailEncodingVisitor::visit(const PropositionalFalse &f) {
  result =
      std::static_pointer_cast<const PropositionalFalse>(f.shared_from_this());
}
void DualRailEncodingVisitor::visit(const PropositionalAtom &f) {
  result = prop_atom(quote(f.shared_from_this()));
}
void DualRailEncodingVisitor::visit(const PropositionalNot &f) {
  result = prop_atom(quote(f.shared_from_this()));
}
void DualRailEncodingVisitor::visit(const PropositionalAnd &f) {
  set_prop_formulas new_container;
  for (const auto &operand : f.get_container()) {
    auto new_operand = apply(*operand);
    new_container.insert(new_operand);
  }
  result = logical_and(new_container);
}
void DualRailEncodingVisitor::visit(const PropositionalOr &f) {
  set_prop_formulas new_container;
  for (const auto &operand : f.get_container()) {
    auto new_operand = apply(*operand);
    new_container.insert(new_operand);
  }
  result = logical_or(new_container);
}

prop_ptr DualRailEncodingVisitor::apply(const PropositionalFormula &b) {
  b.accept(*this);
  return result;
}

// TODO refactor
std::vector<set_atoms_ptr> get_clauses(const PropositionalFormula &f) {
  if (is_a<PropositionalAnd>(f)) {
    std::vector<set_atoms_ptr> result;
    set_prop_formulas set =
        dynamic_cast<const PropositionalAnd &>(f).get_container();
    result.reserve(set.size());
    for (const auto &subf : set) {
      if (is_a<PropositionalOr>(*subf)) {
        auto literals =
            dynamic_cast<const PropositionalOr &>(*subf).get_container();
        auto clause = set_atoms_ptr{};
        bool seen_true = false;
        for (const auto &literal : literals) {
          if (is_a<PropositionalFalse>(*literal))
            continue;
          else if (is_a<PropositionalTrue>(*literal)) {
            seen_true = true;
            break;
          } else
            clause.insert(
                std::static_pointer_cast<const PropositionalAtom>(literal));
        }
        if (not seen_true)
          result.push_back(clause);
      } else {
        result.push_back(set_atoms_ptr(
            {std::static_pointer_cast<const PropositionalAtom>(subf)}));
      }
    }
    return result;
  } else if (is_a<PropositionalOr>(f)) {
    auto literals = dynamic_cast<const PropositionalOr &>(f).get_container();
    auto clause = set_atoms_ptr{};
    for (const auto &literal : literals)
      clause.insert(std::static_pointer_cast<const PropositionalAtom>(literal));
    return std::vector<set_atoms_ptr>({clause});
  } else {
    return std::vector<set_atoms_ptr>(
        {set_atoms_ptr({std::static_pointer_cast<const PropositionalAtom>(
            f.shared_from_this())})});
  }
}

void ClauseExtractorVisitor::visit(const PropositionalTrue &f) {}

void ClauseExtractorVisitor::visit(const PropositionalFalse &f) {}

void ClauseExtractorVisitor::visit(const PropositionalAtom &f) {
  auto ptr =
      std::static_pointer_cast<const PropositionalAtom>(f.shared_from_this());
  if (in_or) {
    current_clause.insert(ptr);
  } else {
    result.push_back(set_atoms_ptr({ptr}));
  }
}

void ClauseExtractorVisitor::visit(const PropositionalAnd &f) {
  for (const auto &subf : f.get_container()) {
    if (is_a<PropositionalFalse>(*subf)) {
      result.clear();
      return;
    }
    subf->accept(*this);
  }
}

void ClauseExtractorVisitor::visit(const PropositionalOr &f) {
  in_or = true;
  for (const auto &subf : f.get_container()) {
    if (is_a<PropositionalTrue>(*subf)) {
      current_clause.clear();
      in_or = false;
      return;
    }
    subf->accept(*this);
  }
  result.push_back(current_clause);
  current_clause.clear();
  in_or = false;
}

void ClauseExtractorVisitor::visit(const PropositionalNot &f) { assert(false); }

void ClauseExtractorVisitor::apply(const PropositionalFormula &f) {
  f.accept(*this);
}

std::vector<set_atoms_ptr> all_prime_implicants(const PropositionalFormula &f) {
  // compute the
  auto cnf_f = to_cnf(f);
  auto visitor = DualRailEncodingVisitor();
  prop_ptr renamed_f = visitor.apply(*cnf_f);

  // now we have a CNF formula whose atoms are positive literal
  //  std::vector<atom_ptr> varindex2atom;
  std::map<atom_ptr, int, SharedComparator> literal2litindex;
  std::vector<atom_ptr> literals;
  set_prop_formulas vars;
  std::vector<std::set<int>> litindex2clauses;

  auto clause_extractor = ClauseExtractorVisitor();
  clause_extractor.apply(*renamed_f);
  std::vector<set_atoms_ptr> clauses = clause_extractor.result;

  // populate indexes
  for (std::size_t clause_index = 0; clause_index < clauses.size();
       ++clause_index) {
    const auto &clause = clauses[clause_index];
    for (const auto &literal : clause) {
      auto it = literal2litindex.find(literal);
      size_t litindex;
      if (it == literal2litindex.end()) {
        litindex = literal2litindex.size();
        literal2litindex[literal] = litindex;
        litindex2clauses.emplace_back();
        literals.push_back(literal);
      } else {
        litindex = literal2litindex[literal];
      }
      litindex2clauses[litindex].insert(clause_index);
      auto var = std::static_pointer_cast<const PropositionalFormula>(
          dynamic_cast<const QuotedFormula &>(*literal->symbol).formula);
      if (is_a<PropositionalNot>(*var)) {
        var = dynamic_cast<const PropositionalNot &>(*var).get_arg();
      }
      vars.insert(var);
    }
  }

  // build dual rail formula
  set_prop_formulas dual_rail;
  for (const auto &var : vars) {
    auto positive_literal = prop_atom(quote(var));
    auto negative_literal = prop_atom(quote(logical_not(var)));
    auto or_ = logical_or(
        {logical_not(positive_literal), logical_not(negative_literal)});
    dual_rail.insert(or_);

    if (literal2litindex.find(positive_literal) == literal2litindex.end()) {
      size_t litindex = literal2litindex.size();
      literal2litindex[positive_literal] = litindex;
      litindex2clauses.emplace_back();
      literals.push_back(positive_literal);
    }
    if (literal2litindex.find(negative_literal) == literal2litindex.end()) {
      size_t litindex = literal2litindex.size();
      literal2litindex[negative_literal] = litindex;
      litindex2clauses.emplace_back();
      literals.push_back(negative_literal);
    }
  }
  auto dual_rail_formula = logical_and(dual_rail);

  // build M(phi_R) formula
  set_prop_formulas and_formulas;
  for (size_t litindex = 0; litindex < literals.size(); ++litindex) {
    set_prop_formulas clause_formulas;
    const auto &literal = literals[litindex];
    const auto &clause_indexes = litindex2clauses[litindex];
    for (const auto &clause_index : clause_indexes) {
      const auto &clause = clauses[clause_index];
      auto reduced_clause = set_prop_formulas(clause.begin(), clause.end());
      reduced_clause.erase(literal);
      clause_formulas.insert(logical_or(reduced_clause));
    }
    // check Jabbour et al. 2014
    auto not_cl = logical_not(logical_and(clause_formulas));
    auto impl = logical_or({logical_not(literal), not_cl});
    and_formulas.insert(impl);
  }
  auto m_formula = logical_and(and_formulas);

  auto final_formula =
      logical_and(set_prop_formulas({renamed_f, dual_rail_formula, m_formula}));
  auto models = all_models<SATModelEnumerationStategy>(*final_formula);
  //    auto models = all_models<NaiveModelEnumerationStategy>(*final_formula);
  return models;
}

void BDDVisitor::visit(const PropositionalTrue &f) {
  result = automaton->mgr.bddOne();
}
void BDDVisitor::visit(const PropositionalFalse &f) {
  result = automaton->mgr.bddZero();
}
void BDDVisitor::visit(const PropositionalAtom &f) {
  atom_ptr atom =
      std::static_pointer_cast<const PropositionalAtom>(f.shared_from_this());
  result = automaton->prop2bddvar(atom2index.at(atom), true);
}
void BDDVisitor::visit(const PropositionalAnd &f) {
  CUDD::BDD res = automaton->mgr.bddOne();
  for (const auto &subf : f.get_args()) {
    res *= apply(*subf);
  }
  result = res;
}
void BDDVisitor::visit(const PropositionalOr &f) {
  CUDD::BDD res = automaton->mgr.bddZero();
  for (const auto &subf : f.get_args()) {
    res += apply(*subf);
  }
  result = res;
}
void BDDVisitor::visit(const PropositionalNot &f) {
  result = !apply(*f.get_arg());
}

CUDD::BDD BDDVisitor::apply(const PropositionalFormula &f) {
  f.accept(*this);
  return result;
}

} // namespace lydia
} // namespace whitemech