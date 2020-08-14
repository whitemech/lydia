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
#include <mona/dfa.h>
}

namespace whitemech {
namespace lydia {

/*
 * Wrapper to MONA DFA.
 */
class mona_dfa : public abstract_dfa {
private:
  DFA *dfa_;

public:
  std::vector<int> indices;

  mona_dfa(DFA *dfa);

  int get_initial_state() const override;
  int get_nb_states() const override;
  int get_nb_variables() const override;

  bool accepts(const trace &word) const override{};

  int get_successor(int state, const interpretation &symbol) const override{};
  int get_successor(int state,
                    const interpretation_set &symbol) const override{};

  virtual bool is_final(int state) const {};

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

} // namespace lydia
} // namespace whitemech