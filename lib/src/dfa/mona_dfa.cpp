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

#include <iostream>
#include <lydia/dfa/mona_dfa.hpp>

namespace whitemech {
namespace lydia {

int mona_dfa::get_nb_states() const { return dfa_->ns; }

int mona_dfa::get_initial_state() const { return dfa_->s; }

int mona_dfa::get_nb_variables() const { return nb_variables_; }

mona_dfa::~mona_dfa() { dfaFree(this->dfa_); }

bool mona_dfa::is_final(int state) const { return dfa_->f[state] == 1; }

int mona_dfa::get_successor(int state, const interpretation &symbol) const {
  bdd_manager *mgr = this->dfa_->bddm;
  unsigned l, r, index;
  unsigned current_node = dfa_->q[state];
  LOAD_lri(&mgr->node_table[current_node], l, r, index);
  while (index != BDD_LEAF_INDEX) {
    current_node = symbol[index] ? r : l;
    LOAD_lri(&mgr->node_table[current_node], l, r, index);
  }
  return l;
}

std::string get_path_guard(int n, trace_descr tp) {
  auto result = std::string(n, 'X');
  trace_descr cur_node = tp;
  while (cur_node) {
    result[cur_node->index] = cur_node->value ? '1' : '0';
    cur_node = cur_node->next;
  }
  return std::string(result);
}

DFA *dfa_concatenate(DFA *a, DFA *b, int n, int *indices) {
  DFA *result;
  DFA *tmp;
  paths state_paths, pp;
  std::string statuses;

  const int new_len = n + 1; // add temporary extra bit, will be projected later
  const int new_var = new_len - 1;
  auto new_indices = std::vector<int>(indices, indices + n);
  new_indices.push_back(new_var);

  const int ns1 = a->ns;
  const int ns2 = b->ns;
  // we add 1 to count the new sink.
  const int new_ns = ns1 + ns2 + 1;
  const int new_sink = new_ns - 1;
  const int ns2_offset = ns1;
  dfaSetup(new_ns, new_len, new_indices.data());

  // mapping from initial states of second automaton to successors.
  auto b_initial_state_ougoing_transitions =
      std::vector<std::pair<int, std::string>>();
  const int b_initial_state = b->s;
  const bool is_b_initial_state_final = b->f[b_initial_state] == 1;
  state_paths = pp = make_paths(b->bddm, b->q[b_initial_state]);

  // save outgoing transitions from initial state of automaton b.
  while (pp) {
    auto guard = get_path_guard(n, pp->trace) + "1";
    b_initial_state_ougoing_transitions.emplace_back(pp->to, guard);
    pp = pp->next;
  }
  kill_paths(state_paths);

  // copy transitions of automaton a
  for (int i = 0; i < a->ns; i++) {
    int next_state;
    std::string next_guard;
    bool is_current_state_final = a->f[i] == 1;
    auto transitions = std::vector<std::pair<int, std::string>>();
    state_paths = pp = make_paths(a->bddm, a->q[i]);
    while (pp) {
      auto guard = get_path_guard(n, pp->trace) + "0";
      transitions.emplace_back(pp->to, guard);
      pp = pp->next;
    }
    int nb_transitions =
        is_current_state_final
            ? transitions.size() + b_initial_state_ougoing_transitions.size()
            : transitions.size();
    statuses +=
        is_current_state_final ? (is_b_initial_state_final ? "+" : "-") : "-";
    dfaAllocExceptions(nb_transitions);
    for (const auto &p : transitions) {
      std::tie(next_state, next_guard) = p;
      dfaStoreException(next_state, next_guard.data());
    }
    // if the current state of automaton a is final, add the new transitions.
    if (is_current_state_final) {
      for (const auto &p : b_initial_state_ougoing_transitions) {
        std::tie(next_state, next_guard) = p;
        int true_next_state =
            next_state == b_initial_state ? i : next_state + ns2_offset;
        dfaStoreException(true_next_state, next_guard.data());
      }
    }
    dfaStoreState(new_sink);
    kill_paths(state_paths);
  }
  // copy transitions of automaton b
  for (int i = 0; i < b->ns; i++) {
    int next_state;
    std::string next_guard;
    state_paths = pp = make_paths(b->bddm, b->q[i]);
    bool is_current_state_final = b->f[i] == 1;
    auto transitions = std::vector<std::pair<int, std::string>>();
    while (pp) {
      auto guard = get_path_guard(n, pp->trace) + "0";
      transitions.emplace_back(pp->to, guard);
      pp = pp->next;
    }
    int nb_transitions = transitions.size();
    statuses += is_current_state_final ? "+" : "-";
    dfaAllocExceptions(nb_transitions);
    for (const auto &p : transitions) {
      std::tie(next_state, next_guard) = p;
      dfaStoreException(next_state + ns2_offset, next_guard.data());
    }
    dfaStoreState(new_sink);
    kill_paths(state_paths);
  }

  // store new sink state.
  dfaAllocExceptions(0);
  dfaStoreState(new_sink);
  statuses += "-";

  result = dfaBuild(statuses.data());
  tmp = dfaProject(result, new_var);
  dfaFree(result);
  result = dfaMinimize(tmp);
  dfaFree(tmp);
  return result;
}

DFA *dfa_closure(DFA *a, int n, int *indices) {
  DFA *result;
  DFA *tmp;
  paths state_paths, pp;
  std::string statuses;

  const int new_len = n + 1; // add temporary extra bit, will be projected later
  const int new_var = new_len - 1;
  auto new_indices = std::vector<int>(indices, indices + n);
  new_indices.push_back(new_var);

  int new_ns = a->ns + +1; // we add a new sink state
  int new_sink = a->ns;
  dfaSetup(new_ns, new_len, new_indices.data());

  // construct the added paths
  auto initial_state_ougoing_transitions =
      std::vector<std::pair<int, std::string>>();
  const int initial_state = a->s;
  state_paths = pp = make_paths(a->bddm, a->q[initial_state]);

  while (pp) {
    auto guard = get_path_guard(n, pp->trace) + "1";
    initial_state_ougoing_transitions.emplace_back(pp->to, guard);
    pp = pp->next;
  }
  kill_paths(state_paths);

  for (int i = 0; i < a->ns; i++) {
    int next_state;
    std::string next_guard;
    bool is_current_state_final = a->f[i] == 1;
    auto transitions = std::vector<std::pair<int, std::string>>();
    state_paths = pp = make_paths(a->bddm, a->q[i]);
    while (pp) {
      auto guard = get_path_guard(n, pp->trace) + "0";
      transitions.emplace_back(pp->to, guard);
      pp = pp->next;
    }
    int nb_transitions =
        is_current_state_final
            ? transitions.size() + initial_state_ougoing_transitions.size()
            : transitions.size();
    statuses += is_current_state_final ? "+" : "-";

    dfaAllocExceptions(nb_transitions);
    for (const auto &p : transitions) {
      std::tie(next_state, next_guard) = p;
      dfaStoreException(next_state, next_guard.data());
    }
    // if the current state of automaton a is final, add the new transitions.
    if (is_current_state_final) {
      for (const auto &p : initial_state_ougoing_transitions) {
        std::tie(next_state, next_guard) = p;
        dfaStoreException(next_state, next_guard.data());
      }
    }
    dfaStoreState(new_sink);
    kill_paths(state_paths);
  }

  // store new sink state.
  dfaAllocExceptions(0);
  dfaStoreState(new_sink);
  statuses += "-";

  result = dfaBuild(statuses.data());
  tmp = dfaProject(result, new_var);
  dfaFree(result);
  result = dfaMinimize(tmp);
  dfaFree(tmp);
  return result;
}

void dfa_accept_empty(DFA *x) { x->f[x->s] = 1; }

DFA *dfaLDLfTrue() {
  dfaSetup(1, 0, nullptr);

  dfaAllocExceptions(0);
  dfaStoreState(0);
  return dfaBuild("+");
}

DFA *dfaLDLfFalse() {
  dfaSetup(1, 0, nullptr);

  dfaAllocExceptions(0);
  dfaStoreState(0);
  return dfaBuild("-");
}

DFA *dfaNext(int a) {
  int var_index[1];
  var_index[0] = a;

  dfaSetup(3, 1, var_index);

  /* boolvar */
  dfaAllocExceptions(1);
  dfaStoreException(1, "1");
  dfaStoreState(2);

  /* state 1 */
  dfaAllocExceptions(0);
  dfaStoreState(2);

  /* state 2 */
  dfaAllocExceptions(0);
  dfaStoreState(2);

  return dfaBuild("-+-");
}

DFA *dfaLDLfDiamondProp(DFA *prop_regex, DFA *body, int var, int *indices) {
  DFA *diamond = dfa_concatenate(prop_regex, body, var, indices);
  return diamond;
}

DFA *dfaPropositionalTrue() {
  dfaSetup(3, 0, nullptr);

  /* boolvar */
  dfaAllocExceptions(0);
  dfaStoreState(1);

  /* state 1 */
  dfaAllocExceptions(0);
  dfaStoreState(2);

  /* state 2 */
  dfaAllocExceptions(0);
  dfaStoreState(2);

  return dfaBuild("-+-");
}

} // namespace lydia
} // namespace whitemech