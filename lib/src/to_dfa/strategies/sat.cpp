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
#include <lydia/pl/models/base.hpp>
#include <lydia/to_dfa/delta_symbolic.hpp>
#include <lydia/to_dfa/strategies/sat.hpp>

namespace whitemech {
namespace lydia {

struct cmp_set_of_ptr {
  template <typename T, typename U>
  bool operator()(const std::set<T, U> &a, const std::set<T, U> &b) const {
    return unified_compare(a, b) < 0;
  }
};

dfa_ptr SATStrategy::to_dfa(const LDLfFormula &formula) {
  //  build initial state of the DFA.
  auto formula_nnf = to_nnf(formula);
  set_formulas initial_state_formulas{formula_nnf};
  dfa_state_ptr initial_state =
      std::make_shared<DFAState>(initial_state_formulas);

  // find all atoms
  set_atoms_ptr atoms = find_atoms(*formula_nnf);
  map_atoms_ptr atom2index;
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
    // push this inside "next_transitions"
    set_atoms_ptr all_symbols_current_transition;
    for (const auto &symbol_state : next_transitions) {
      all_symbols_current_transition.insert(symbol_state.first.begin(),
                                            symbol_state.first.end());
    }

    for (const auto &symbol_state : next_transitions) {
      const auto &symbol = symbol_state.first;
      const auto &next_state = symbol_state.second;
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

      interpretation_map x;
      for (const atom_ptr &atom : all_symbols_current_transition)
        x[atom2index[atom]] = symbol.find(atom) != symbol.end();
      automaton->add_transition(current_state_index, x, next_state_index);
    }
  }

  return automaton;
}

std::vector<std::pair<set_atoms_ptr, dfa_state_ptr>>
SATStrategy::next_transitions(const DFAState &state) {
  std::vector<std::pair<set_atoms_ptr, dfa_state_ptr>> result;
  std::map<set_atoms_ptr, set_nfa_states, cmp_set_of_ptr> symbol2nfastates;
  set_dfa_states discovered;
  set_nfa_states nfa_states;
  set_atoms_ptr symbol;
  for (const auto &nfa_state : state.states) {
    const auto &next_transitions = this->next_transitions(*nfa_state);
    for (const auto &symbol_states : next_transitions) {
      symbol = symbol_states.first;
      nfa_states = symbol_states.second;
      if (symbol2nfastates.find(symbol) == symbol2nfastates.end()) {
        symbol2nfastates[symbol] = set_nfa_states{};
      }
      symbol2nfastates[symbol].insert(nfa_states.begin(), nfa_states.end());
    }
  }

  result.reserve(symbol2nfastates.size());
  for (const auto &pair : symbol2nfastates) {
    result.emplace_back(pair.first, std::make_shared<DFAState>(pair.second));
  }
  return result;
}

//// only works if formula is in CNF
// void update_model(operations_research::sat::CpModelBuilder& cp_model,
//    const PropositionalFormula& f,
//    const std::map<prop_ptr, operations_research::sat::BoolVar,
//    SharedComparator>& atom2var){
//  if (is_a<PropositionalTrue>(f)){
//    cp_model.AddBoolOr({cp_model.TrueVar()});
//  }
//  else if (is_a<PropositionalFalse>(f)){
//    cp_model.AddBoolAnd({cp_model.FalseVar()});
//  }
//  else if (is_a<PropositionalAtom>(f)){
//    auto casted_p = std::static_pointer_cast<const
//    PropositionalAtom>(f.shared_from_this()); auto var =
//    atom2var.at(casted_p); cp_model.AddBoolAnd({var});
//  }
//  else if (is_a<PropositionalNot>(f)){
//    const auto& casted_p = dynamic_cast<const PropositionalNot&>(f);
//    auto var = atom2var.at(casted_p.get_arg());
//    cp_model.AddBoolAnd({Not(var)});
//  }
//  else if (is_a<PropositionalOr>(f)){
//    const auto& or_ = dynamic_cast<const PropositionalOr&>(f);
//    std::vector<operations_research::sat::BoolVar> or_of_vars;
//    or_of_vars.reserve(or_.get_container().size());
//    for (const auto& p :or_.get_container()){
//      if (is_a<PropositionalTrue>(*p))
//        or_of_vars.push_back({cp_model.TrueVar()});
//      else if(is_a<PropositionalFalse>(*p))
//        or_of_vars.push_back({cp_model.FalseVar()});
//      else if(is_a<PropositionalNot>(*p)){
//        const auto& casted_p = dynamic_cast<const PropositionalNot&>(*p);
//        auto var = atom2var.at(casted_p.get_arg());
//        or_of_vars.push_back({Not(var)});
//      }
//      else{
//        auto casted_p = std::static_pointer_cast<const PropositionalAtom>(p);
//        or_of_vars.push_back(atom2var.at(casted_p));
//      }
//    }
//    cp_model.AddBoolOr(or_of_vars);
//  }
//  else if (is_a<PropositionalAnd>(f)){
//    const auto& and_ = dynamic_cast<const PropositionalAnd&>(f);
//    std::vector<operations_research::sat::BoolVar> and_of_vars;
//    for (const auto& subf: and_.get_container()){
//      if (is_a<PropositionalOr>(*subf)){
//        update_model(cp_model, *subf, atom2var);
//      }
//      else if (is_a<PropositionalTrue>(*subf))
//        and_of_vars.push_back({cp_model.TrueVar()});
//      else if(is_a<PropositionalFalse>(*subf))
//        and_of_vars.push_back({cp_model.FalseVar()});
//      else if(is_a<PropositionalNot>(*subf)){
//        const auto& casted_p = dynamic_cast<const PropositionalNot&>(*subf);
//        auto var = atom2var.at(casted_p.get_arg());
//        and_of_vars.push_back({Not(var)});
//      }
//      else{
//        auto casted_p = std::static_pointer_cast<const
//        PropositionalAtom>(subf);
//        and_of_vars.push_back(atom2var.at(casted_p));
//      }
//    }
//    cp_model.AddBoolAnd(and_of_vars);
//  }
//
//}

std::vector<std::pair<set_atoms_ptr, set_nfa_states>>
SATStrategy::next_transitions(const NFAState &state) {
  set_prop_formulas setPropFormulas;
  for (const auto &f : state.formulas) {
    const auto &delta_formula = delta_symbolic(*f, false);
    setPropFormulas.insert(delta_formula);
  }
  auto and_ = logical_and(setPropFormulas);
  return this->next_transitions_from_delta_formula(*and_);
}

std::vector<std::pair<set_atoms_ptr, set_nfa_states>>
SATStrategy::next_transitions_from_delta_formula(
    const PropositionalFormula &f) {
  std::vector<std::pair<set_atoms_ptr, set_nfa_states>> result;
  std::map<set_atoms_ptr, set_nfa_states, cmp_set_of_ptr> symbol2nfastates;
  set_atoms_ptr quoted_formulas;
  set_atoms_ptr propositionals;
  auto atoms = find_atoms(f);
  return result;
}

//  std::vector<std::pair<set_atoms_ptr, set_nfa_states>>
//  SATStrategy::next_transitions_from_delta_formula(
//      const PropositionalFormula &f) {
//    std::vector<std::pair<set_atoms_ptr, set_nfa_states>> result;
//    std::map<set_atoms_ptr, set_nfa_states, cmp_set_of_ptr> symbol2nfastates;
//
//    set_atoms_ptr quoted_formulas;
//    set_atoms_ptr propositionals;
//    auto atoms = find_atoms(f);
//    std::map<prop_ptr, operations_research::sat::BoolVar, SharedComparator>
//    atom2var; std::vector<operations_research::sat::BoolVar> all_vars;
//    std::vector<prop_ptr> idx2atom;
//    idx2atom.reserve(atoms.size());
//    all_vars.reserve(atoms.size());
//
//    //build SAT problem
//    operations_research::sat::CpModelBuilder cp_model;
//    for (const atom_ptr &ptr : atoms) {
//      std::string name;
//      if (is_a<QuotedFormula>(*ptr->symbol)) {
//        quoted_formulas.insert(ptr);
//        name = "q_" + std::to_string(quoted_formulas.size());
//      }
//      else {
//        propositionals.insert(ptr);
//        name = "p_" + std::to_string(propositionals.size());
//      }
//      auto var = cp_model.NewBoolVar().WithName(name);
//      atom2var[ptr] = var;
//      idx2atom.push_back(ptr);
//      all_vars.push_back(var);
//    }
//    update_model(cp_model, f, atom2var);
//    operations_research::sat::Model model;
//    int num_solutions = 0;
//    model.Add(operations_research::sat::NewFeasibleSolutionObserver
//                  ([&](const operations_research::sat::CpSolverResponse&
//                  response) {
//                    LOG(INFO) << "Solution " << num_solutions;
//                    num_solutions++;
//                  }));
//    operations_research::sat::SatParameters parameters;
//    parameters.set_enumerate_all_solutions(true);
//    model.Add(NewSatParameters(parameters));
//    operations_research::sat::TableConstraint t =
//        cp_model.AddForbiddenAssignments(all_vars);
//
//    std::vector<std::vector<int64_t>> solutions;
//    operations_research::sat::CpSolverResponse response =
//    Solve(cp_model.Build()); while (response.status() ==
//    operations_research::sat::CpSolverStatus::FEASIBLE){
//      std::vector<int64_t> solution;
//      solution.reserve(atom2var.size());
//      for (const auto& atom_var_pair : atom2var){
//        solution.push_back(SolutionIntegerValue(response,
//        atom_var_pair.second));
//      }
//      solutions.push_back(solution);
//    }
//    const operations_research::sat::CpSolverResponse response =
//    SolveCpModel(cp_model.Build(), &model); LOG(INFO) << "Number of solutions
//    found: " << num_solutions; std::cout<< "hello"<<std::endl;
//
//    auto symbols = powerset(propositionals);
//    for (const auto &symbol : symbols) {
//      std::map<prop_ptr, prop_ptr, SharedComparator> replacements;
//      for (const auto &x : propositionals) {
//        replacements[x] = boolean_prop(!(symbol.end() == symbol.find(x)));
//      }
//      set_nfa_states nfa_states;
//      symbol2nfastates[symbol] = set_nfa_states();
//
//      auto replaced_delta = replace(replacements, f);
//      const auto &all_minimal_models = all_models(*replaced_delta);
//      for (const auto &model : all_minimal_models) {
//        set_formulas current_formulas;
//        for (const auto &current_quoted_formula : model) {
//          current_formulas.insert(
//              dynamic_cast<const QuotedFormula
//              &>(*current_quoted_formula->symbol)
//                  .formula);
//        }
//        symbol2nfastates[symbol].insert(
//            std::make_shared<NFAState>(current_formulas));
//      }
//    }
//
//    result.reserve(symbol2nfastates.size());
//    for (const auto &pair : symbol2nfastates) {
//      result.emplace_back(pair);
//    }
//    return result;
//  }

} // namespace lydia
} // namespace whitemech