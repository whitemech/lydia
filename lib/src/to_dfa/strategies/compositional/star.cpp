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

#include <lydia/logic/pl/eval.hpp>
#include <lydia/logic/pl/prop_to_bdd.hpp>
#include <lydia/to_dfa/strategies/compositional/delta_compositional.hpp>
#include <lydia/to_dfa/strategies/compositional/star.hpp>
#include <lydia/utils/cudd.hpp>
#include <lydia/utils/strings.hpp>
#include <queue>

namespace whitemech::lydia {

typedef std::pair<prop_ptr, ldlf_ptr> transition;

DFA* remove_unreachable_states(DFA* arg) {
  DFA* result;
  DFA* tmp = dfaLDLfTrue();
  result = dfaProduct(arg, tmp, dfaAND);
  dfaFree(tmp);
  tmp = dfaMinimize(result);
  dfaFree(result);
  result = tmp;
  return result;
}

DFA* CompositionalStrategy::star(const RegExp& r, DFA* body) {
  if (!is_a<StarRegExp>(r)) {
    throw std::invalid_argument("Only star regex allowed.");
  }

  CUDD::Cudd mgr = CUDD::Cudd(0, 0, 0, 0, 0);

  auto prop_to_bdd_visitor = PropToBDDVisitor(*prop_mgr, this->id2atoms);
  auto regex = std::static_pointer_cast<const RegExp>(r.shared_from_this());
  auto q_argument =
      body->f[body->s] == 1 ? r.ctx().makeLdlfTrue() : r.ctx().makeLdlfFalse();
  auto formula = r.ctx().makeLdlfDiamond(regex, r.ctx().makeLdlfQ(q_argument));
  std::string statuses;

  size_t max_and_bits = 0;
  size_t max_or_bits = 0;
  std::map<ldlf_ptr, size_t, SharedComparator> test2id;
  std::vector<ldlf_ptr> id2test;
  std::vector<DFA*> test_formula_to_dfa;

  // build the entire graph first, used later to build the automaton.
  size_t next_state_index = 1;
  std::map<ldlf_ptr, size_t, SharedComparator> state2id;
  std::vector<ldlf_ptr> id2state;
  set_formulas discovered{{formula}};
  // map from states (i.e. ldlf formulae) to
  // vector of vector of transitions;
  //    - the first level determines the transitions in "existential
  //    nondeterminism";
  //    - the second level determines the transitions in "universal
  //    nondeterminism".
  // the transition guard might be either a propositional or null (concatenation
  // with next DFA).
  std::map<size_t, std::vector<std::vector<transition>>> transitions_by_state;
  std::queue<std::pair<ldlf_ptr, size_t>> to_be_visited;
  DeltaCompositionalVisitor visitor{mgr};

  to_be_visited.push(std::make_pair(formula, 0));
  while (!to_be_visited.empty()) {
    auto pair = to_be_visited.front();
    auto current_state = pair.first;
    auto state_id = pair.second;
    id2state.push_back(current_state);
    state2id[current_state] = state_id;

    to_be_visited.pop();

    auto result = visitor.apply(*current_state);
    std::vector<std::vector<uint8_t>> primes =
        get_primes(result, visitor.id2subformula.size());

    std::vector<std::vector<transition>> existential_transitions;
    for (const auto& prime : primes) {
      std::vector<transition> universal_transitions;
      for (size_t i = 0; i < prime.size(); i++) {
        if (prime[i] == 1) {
          // check if item is in the form <prop>phi
          auto item = visitor.id2subformula[i];
          prop_ptr guard;
          ldlf_ptr next_formula_state;
          if (is_a<LDLfDiamond>(*item)) {
            auto prop_transition =
                std::static_pointer_cast<const LDLfDiamond>(item);
            guard = std::static_pointer_cast<const PropositionalRegExp>(
                        prop_transition->get_regex())
                        ->get_arg();
            next_formula_state = prop_transition->get_formula();
            // check for addition only if LDLfDiamond
            if (discovered.find(next_formula_state) == discovered.end()) {
              // new state - add it to the queue
              discovered.insert(next_formula_state);
              to_be_visited.push(
                  std::make_pair(next_formula_state, next_state_index));
              next_state_index += 1;
            }
          }
          // either a test expression
          else if (is_a<TestRegExp>(*item)) {
            guard = nullptr;
            next_formula_state =
                std::static_pointer_cast<const TestRegExp>(item)->get_arg();
            if (test2id.find(next_formula_state) == test2id.end()) {
              auto new_id = id2test.size();
              test2id[next_formula_state] = new_id;
              id2test.push_back(next_formula_state);
            }
          }
          // or auxiliary construct Q.
          else {
            assert(is_a<LDLfQ>(*item));
            guard = nullptr;
            next_formula_state = std::static_pointer_cast<const LDLfQ>(item);
          }
          universal_transitions.emplace_back(guard, next_formula_state);
        }
      }

      if (size_t nb_ands = universal_transitions.size(); nb_ands > 1)
        max_and_bits =
            std::max(max_and_bits, (unsigned long)ceil(log2(nb_ands)));

      existential_transitions.push_back(universal_transitions);
    }
    if (size_t nb_ors = existential_transitions.size(); nb_ors > 1)
      max_or_bits = std::max(max_or_bits, (unsigned long)ceil(log2(nb_ors)));
    transitions_by_state[state_id] = existential_transitions;
  }

  test_formula_to_dfa.resize(test2id.size());
  for (const auto& pair : test2id) {
    const auto& test_expr = pair.first;
    auto compose_visitor = ComposeDFAVisitor(*this);
    DFA* tmp = compose_visitor.apply(*test_expr);
    test_formula_to_dfa[pair.second] = tmp;
  }

  // ----------------------------------------------------------------
  // start automaton building
  // ----------------------------------------------------------------

  // compute new indices
  auto new_indices =
      std::vector<int>(indices.size() + max_and_bits + max_or_bits);
  auto n = indices.size();
  auto start = new_indices.begin();
  std::copy(indices.begin(), indices.end(), start);
  std::iota(start + n, start + n + max_or_bits + max_and_bits, n);

  // compute new number of states
  size_t new_ns = 0;
  new_ns += discovered.size();
  // sum the other states
  new_ns += body->ns;
  std::for_each(test_formula_to_dfa.begin(), test_formula_to_dfa.end(),
                [&new_ns](const auto& dfa_ptr) { new_ns += dfa_ptr->ns; });
  // for the sink
  new_ns += 2;
  auto accepting_sink = new_ns - 2;
  auto rejecting_sink = new_ns - 1;

  // compute offsets for test and phi automata states
  // last index is for body offset
  auto offsets = std::vector<int>(id2test.size() + 1);
  auto current_offset = discovered.size();
  for (size_t i = 0; i < offsets.size(); i++) {
    offsets[i] = current_offset;
    current_offset +=
        (i != id2test.size() ? test_formula_to_dfa[i]->ns : body->ns);
  }

  // precompute transitions from initial state for each automata.
  paths state_paths, pp;
  auto test_initial_state_outgoing_transitions =
      std::vector<std::vector<std::pair<int, std::string>>>(offsets.size());

  for (size_t i = 0; i < offsets.size(); i++) {
    auto dfa_ptr =
        i != test_formula_to_dfa.size() ? test_formula_to_dfa[i] : body;
    state_paths = pp = make_paths(dfa_ptr->bddm, dfa_ptr->q[dfa_ptr->s]);
    while (pp) {
      auto guard = get_path_guard(indices.size(), pp->trace);
      test_initial_state_outgoing_transitions[i].emplace_back(pp->to, guard);
      pp = pp->next;
    }
    kill_paths(state_paths);
  }

  dfaSetup(new_ns, new_indices.size(), new_indices.data());
  size_t state_id;
  std::vector<std::vector<transition>> next_transitions;
  for (const auto& state_transitions_pair : transitions_by_state) {
    std::vector<std::pair<int, std::string>> exceptions;
    std::tie(state_id, next_transitions) = state_transitions_pair;
    size_t nb_or_bits = ceil(log2(next_transitions.size()));
    for (size_t i = 0; i < next_transitions.size(); ++i) {
      const auto universal_transitions = next_transitions[i];
      auto auxiliary_or_guard = next_transitions.size() == 1
                                    ? std::string(max_or_bits, 'X')
                                    : state2bin(i, nb_or_bits, true);
      size_t nb_and_bits = ceil(log2(universal_transitions.size()));
      for (size_t j = 0; j < universal_transitions.size(); ++j) {
        const auto transition = universal_transitions[j];
        auto auxiliary_and_guard = universal_transitions.size() == 1
                                       ? std::string(max_and_bits, 'X')
                                       : state2bin(j, nb_and_bits, true);
        auto auxiliary_or_and_guard = auxiliary_or_guard + auxiliary_and_guard;
        auto prop = transition.first;
        auto next_state = transition.second;
        if (prop == nullptr) {
          // is a test or Q
          size_t index =
              is_a<LDLfQ>(*next_state) ? id2test.size() : test2id[next_state];
          size_t offset = offsets[index];
          auto next_subtransitions =
              test_initial_state_outgoing_transitions[index];
          for (const auto& subtransition : next_subtransitions) {
            const auto next_subautomaton_state = subtransition.first + offset;
            auto guard = subtransition.second + auxiliary_or_and_guard;
            exceptions.emplace_back(next_subautomaton_state, guard);
          }
        } else {
          auto prop_bdd = prop_to_bdd_visitor.apply(*prop);
          const auto prop_cubes = get_cubes(prop_bdd, this->id2atoms.size());
          for (const auto& cube : prop_cubes) {
            std::vector<int> cube_int(cube.begin(), cube.end());
            std::vector<char> cube_guard(cube_int.size());
            std::transform(cube_int.begin(), cube_int.end(), cube_guard.begin(),
                           [](const int& c) {
                             return c == 0 ? '0' : (c == 1 ? '1' : 'X');
                           });
            std::string cube_string(cube_guard.begin(), cube_guard.end());
            cube_string = cube_string + auxiliary_or_and_guard;
            const auto next_state_id = state2id[next_state];
            exceptions.emplace_back(next_state_id, cube_string);
          }
        }

        for (size_t jj = universal_transitions.size();
             jj < (size_t)pow(2, nb_and_bits); ++jj) {
          auto auxiliary_default_and_guard =
              state2bin(jj, nb_and_bits, true) +
              std::string(max_and_bits - nb_and_bits, 'X');
          auto dont_care_variables = std::string(indices.size(), 'X');
          auto auxiliary_default_guard = dont_care_variables +
                                         auxiliary_or_guard +
                                         auxiliary_default_and_guard;
          exceptions.emplace_back(accepting_sink, auxiliary_default_guard);
        }
      }
    }

    // finally alloc exceptions
    dfaAllocExceptions(exceptions.size());
    for (auto& exception : exceptions) {
      dfaStoreException(exception.first, exception.second.data());
    }

    auto state_formula = id2state[state_id];
    auto delta_visitor = DeltaVisitor{set_atoms_ptr{}, true};
    auto delta_epsilon_output = delta_visitor.apply(*state_formula);
    auto is_final = eval(*delta_epsilon_output, set_atoms_ptr{});
    statuses += is_final ? "+" : "-";
    dfaStoreState(rejecting_sink);
  }

  auto auxiliary_guard_x = std::string(max_or_bits + max_and_bits, 'X');
  // copy all the other automata
  for (size_t automaton_id = 0; automaton_id < offsets.size(); automaton_id++) {
    DFA* subautomaton = automaton_id == id2test.size()
                            ? body
                            : test_formula_to_dfa[automaton_id];
    int current_offset = offsets[automaton_id];
    for (int i = 0; i < subautomaton->ns; i++) {
      int next_state;
      std::string next_guard;
      state_paths = pp = make_paths(subautomaton->bddm, subautomaton->q[i]);
      bool is_current_state_final = subautomaton->f[i] == 1;
      auto transitions = std::vector<std::pair<int, std::string>>();
      while (pp) {
        auto guard = get_path_guard(n, pp->trace);
        transitions.emplace_back(pp->to, guard);
        pp = pp->next;
      }
      int nb_transitions = transitions.size();
      statuses += is_current_state_final ? "+" : "-";
      dfaAllocExceptions(nb_transitions);
      for (const auto& p : transitions) {
        std::tie(next_state, next_guard) = p;
        dfaStoreException(next_state + current_offset,
                          next_guard.append(auxiliary_guard_x).data());
      }
      dfaStoreState(rejecting_sink);
      kill_paths(state_paths);
    }
  }

  // store new accepting sink state.
  dfaAllocExceptions(0);
  dfaStoreState(accepting_sink);
  statuses += "+";

  // store new rejecting sink state.
  dfaAllocExceptions(0);
  dfaStoreState(rejecting_sink);
  statuses += "-";

  DFA* result = dfaBuild(statuses.data());
  DFA* tmp = dfaMinimize(result);
  dfaFree(result);
  result = remove_unreachable_states(tmp);
  dfaFree(tmp);

  for (int j = max_and_bits - 1; j >= 0; --j) {
    tmp = dfaUniversalProject(result, indices.size() + max_or_bits + j);
    dfaFree(result);
    result = dfaMinimize(tmp);
    dfaFree(tmp);
  }
  for (int i = max_or_bits - 1; i >= 0; --i) {
    tmp = dfaProject(result, indices.size() + i);
    dfaFree(result);
    result = dfaMinimize(tmp);
    dfaFree(tmp);
  }

  tmp = result;
  result = dfaMinimize(tmp);
  dfaFree(tmp);

  // ----------------------------------------------------------------
  // end automaton building
  // ----------------------------------------------------------------

  // free test DFAs
  std::for_each(test_formula_to_dfa.begin(), test_formula_to_dfa.end(),
                [](const auto& dfa_ptr) { dfaFree(dfa_ptr); });

  return result;
}

} // namespace whitemech::lydia