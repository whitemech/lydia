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

#include <lydia/dfa/abstract_dfa.hpp>

extern "C" {
#include <mona/bdd.h>
#include <mona/dfa.h>
#include <mona/mem.h>
}

namespace whitemech {
namespace lydia {

/*
 * Wrapper to MONA DFA.
 */
class mona_dfa : public abstract_dfa {
private:
  DFA *dfa_;
  int nb_variables_;

public:
  std::vector<int> indices;

  mona_dfa(DFA *dfa, int nb_variables)
      : dfa_{dfa}, nb_variables_{nb_variables} {}
  ~mona_dfa();

  DFA *get_dfa() { return dfa_; }

  int get_initial_state() const override;
  int get_nb_states() const override;
  int get_nb_variables() const override;

  bool accepts(const trace &word) const override {
    int current_state = get_initial_state();
    for (const auto &symbol : word) {
      current_state = get_successor(current_state, symbol);
    }
    return is_final(current_state);
  };

  int get_successor(int state, const interpretation &symbol) const override;
  int get_successor(int state,
                    const interpretation_set &symbol) const override{};

  bool is_final(int state) const override;

  virtual int add_state(){};

  void set_initial_state(int state) override{};

  void set_final_state(int state, bool is_final = true) override{};

  void add_transition(int from, const interpretation_map &symbol,
                      int to) override{};

  void add_transition(int from, const interpretation &symbol, int to,
                      bool dont_care = true) override{};

  void add_transition(int from, const interpretation_set &symbol, int to,
                      bool dont_care = true) override{};
};

DFA *dfa_concatenate(DFA *a, DFA *b, int n, int *indices);
DFA *dfa_closure(DFA *a, int n, int *indices);
void dfa_accept_empty(DFA *x);
std::string get_path_guard(int n, trace_descr tp);

DFA *dfaLDLfTrue();
DFA *dfaLDLfFalse();
DFA *dfaNext(int a);
DFA *dfaLDLfDiamondProp(DFA *prop_regex, DFA *body, int var, int *indices);
DFA *dfaPropositionalTrue();

} // namespace lydia
} // namespace whitemech