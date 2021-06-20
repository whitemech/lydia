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

#include <lydia/mona_ext/mona_ext_base.hpp>

namespace whitemech::lydia {

std::string get_path_guard(int n, trace_descr tp) {
  auto result = std::string(n, 'X');
  trace_descr cur_node = tp;
  while (cur_node) {
    result[cur_node->index] = cur_node->value ? '1' : '0';
    cur_node = cur_node->next;
  }
  return std::string(result);
}

DFA* dfa_concatenate(DFA* a, DFA* b, int n, int* indices) {
  DFA* result;
  DFA* tmp;
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
    for (const auto& p : transitions) {
      std::tie(next_state, next_guard) = p;
      dfaStoreException(next_state, next_guard.data());
    }
    // if the current state of automaton a is final, add the new transitions.
    if (is_current_state_final) {
      for (const auto& p : b_initial_state_ougoing_transitions) {
        std::tie(next_state, next_guard) = p;
        int true_next_state = next_state + ns2_offset;
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
    for (const auto& p : transitions) {
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

DFA* dfa_closure(DFA* a, int n, int* indices) {
  DFA* result;
  DFA* tmp;
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
    for (const auto& p : transitions) {
      std::tie(next_state, next_guard) = p;
      dfaStoreException(next_state, next_guard.data());
    }
    // if the current state of automaton a is final, add the new transitions.
    if (is_current_state_final) {
      for (const auto& p : initial_state_ougoing_transitions) {
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

DFA* only_empty() {
  dfaSetup(2, 0, nullptr);

  dfaAllocExceptions(0);
  dfaStoreState(1);

  dfaAllocExceptions(0);
  dfaStoreState(1);

  return dfaBuild("+-");
}

DFA* dfa_accept_empty(DFA* x) {
  DFA* tmp = only_empty();
  DFA* result = dfaProduct(x, tmp, dfaOR);
  dfaFree(tmp);
  return result;
}

DFA* dfaLDLfTrue() {
  dfaSetup(1, 0, nullptr);

  dfaAllocExceptions(0);
  dfaStoreState(0);
  return dfaBuild("+");
}

DFA* dfaLDLfFalse() {
  dfaSetup(1, 0, nullptr);

  dfaAllocExceptions(0);
  dfaStoreState(0);
  return dfaBuild("-");
}

DFA* dfaLDLfEnd(int var, int* indices) {
  DFA* tmp_1 = dfaPropositionalTrue();
  DFA* tmp_2 = dfaLDLfTrue();
  DFA* d =
      dfaLDLfDiamondProp(dfaPropositionalTrue(), dfaLDLfTrue(), var, indices);
  dfaNegation(d);
  dfaFree(tmp_1);
  dfaFree(tmp_2);
  return d;
}

DFA* dfaNext(int a, bool is_positive) {
  int var_index[1];
  var_index[0] = a;

  dfaSetup(3, 1, var_index);

  /* boolvar */
  dfaAllocExceptions(1);
  dfaStoreException(1,
                    (is_positive ? std::string("1") : std::string("0")).data());
  dfaStoreState(2);

  /* state 1 */
  dfaAllocExceptions(0);
  dfaStoreState(2);

  /* state 2 */
  dfaAllocExceptions(0);
  dfaStoreState(2);

  return dfaBuild("-+-");
}

DFA* dfaLDLfDiamondProp(DFA* prop_regex, DFA* body, int var, int* indices) {
  DFA* diamond = dfa_concatenate(prop_regex, body, var, indices);
  return diamond;
}

DFA* dfaPropositionalTrue() {
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

void print_mona_dfa(DFA* a, const std::string& name, int num) {
  std::vector<unsigned> x(num);
  std::iota(std::begin(x), std::end(x), 0);
  std::ofstream out;
  out.open(name + ".dot", std::ofstream::out);
  dfaPrintGraphvizToFile(a, num, x.data(), out);
  out.close();
  std::system(
      std::string("dot -Tsvg '" + name + ".dot' > '" + name + ".svg'").c_str());
}

void dfaPrintGraphvizToFile(DFA* a, int no_free_vars, unsigned* offsets,
                            std::ostream& o) {
  paths state_paths, pp;
  trace_descr tp;
  int i, j, k, l;
  char** buffer;
  int *used, *allocated;

  o << "digraph MONA_DFA {\n"
       " rankdir = LR;\n"
       " center = true;\n"
       " size = \"7.5,10.5\";\n"
       " edge [fontname = Courier];\n"
       " node [height = .5, width = .5];\n"
       " node [shape = doublecircle];";
  for (i = 0; i < a->ns; i++)
    if (a->f[i] == 1)
      o << " " << i;
  o << "\n node [shape = circle];";
  for (i = 0; i < a->ns; i++)
    if (a->f[i] == -1)
      o << " " << i;
  o << "\n node [shape = box];";
  for (i = 0; i < a->ns; i++)
    if (a->f[i] == 0)
      o << " " << i;
  o << "\n init [shape = plaintext, label = \"\"];\n"
       " init -> "
    << a->s << ";\n";

  buffer = (char**)mem_alloc(sizeof(char*) * a->ns);
  used = (int*)mem_alloc(sizeof(int) * a->ns);
  allocated = (int*)mem_alloc(sizeof(int) * a->ns);

  for (i = 0; i < a->ns; i++) {
    state_paths = pp = make_paths(a->bddm, a->q[i]);

    for (j = 0; j < a->ns; j++) {
      buffer[j] = 0;
      used[j] = allocated[j] = 0;
    }

    while (pp) {
      if (used[pp->to] >= allocated[pp->to]) {
        allocated[pp->to] = allocated[pp->to] * 2 + 2;
        buffer[pp->to] = (char*)mem_resize(
            buffer[pp->to], sizeof(char) * allocated[pp->to] * no_free_vars);
      }

      for (j = 0; j < no_free_vars; j++) {
        char c;
        for (tp = pp->trace; tp && (tp->index != offsets[j]); tp = tp->next)
          ;

        if (tp) {
          if (tp->value)
            c = '1';
          else
            c = '0';
        } else
          c = 'X';

        buffer[pp->to][no_free_vars * used[pp->to] + j] = c;
      }
      used[pp->to]++;
      pp = pp->next;
    }

    for (j = 0; j < a->ns; j++)
      if (buffer[j]) {
        o << " " << i << " -> " << j << " [label=\"";
        for (k = 0; k < no_free_vars; k++) {
          for (l = 0; l < used[j]; l++) {
            o << (buffer[j][no_free_vars * l + k]);
            if (l + 1 < used[j]) {
              if (k + 1 == no_free_vars)
                o << (',');
              else
                o << (' ');
            }
          }
          if (k + 1 < no_free_vars)
            o << "\\n";
        }
        o << "\"];\n";
        mem_free(buffer[j]);
      }

    kill_paths(state_paths);
  }

  mem_free(allocated);
  mem_free(used);
  mem_free(buffer);

  o << "}\n";
}

void dfaPrint(DFA* a, int no_free_vars, std::vector<std::string> free_variables,
              unsigned* offsets, std::ostream& o) {
  paths state_paths, pp;
  trace_descr tp;
  int i, j, any = 0;

  o << "DFA for formula with free variables: ";

  for (i = 0; i < no_free_vars; i++)
    o << free_variables[i] << " ";

  o << "\nInitial state: " << a->s
    << "\n"
       "Accepting states: ";

  for (i = 0; i < a->ns; i++)
    if (a->f[i] == 1)
      o << i << " ";

  o << "\n"
       "Rejecting states: ";

  for (i = 0; i < a->ns; i++)
    if (a->f[i] == -1)
      o << i << " ";

  o << "\n";

  for (i = 0; i < a->ns; i++)
    if (a->f[i] == 0) {
      any = 1;
      break;
    }
  if (any) {
    printf("Don't-care states: ");
    for (i = 0; i < a->ns; i++)
      if (a->f[i] == 0)
        o << i << " ";

    o << "\n";
  }

  o << "\nAutomaton has " << a->ns << " state(s) and " << bdd_size(a->bddm)
    << " BDD-node(s)\n";

  o << "Transitions:\n";

  for (i = 0; i < a->ns; i++) {
    state_paths = pp = make_paths(a->bddm, a->q[i]);

    while (pp) {
      o << "State " << i << ": ";

      for (j = 0; j < no_free_vars; j++) {
        for (tp = pp->trace; tp && (tp->index != offsets[j]); tp = tp->next)
          ;

        if (tp) {
          if (tp->value)
            o << "1";
          else
            o << "0";
        } else
          o << "X";
      }

      o << " -> state " << pp->to << "\n";

      pp = pp->next;
    }

    kill_paths(state_paths);
  }
}

bool is_sink(DFA* automaton, bool is_positive) {
  bool has_one_state = automaton->ns == 1;
  bool is_initial_state_final = automaton->f[0] == 1;
  return has_one_state and (is_initial_state_final == is_positive);
}

} // namespace whitemech::lydia