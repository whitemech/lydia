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

#include <algorithm>
#include <cassert>
#include <lydia/logger.hpp>
#include <lydia/types.hpp>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace whitemech::lydia {

class abstract_dfa {
public:
  virtual int get_initial_state() const = 0;
  virtual int get_nb_states() const = 0;
  virtual int get_nb_variables() const = 0;

  /*!
   * Check whether a word of propositional interpretations
   * is accepted by the DFA.
   *
   * //TODO consider using bit vectors for an interpretation.
   *
   * @return true if the word is accepted, false otherwise.
   */
  virtual bool accepts(const trace& word) const = 0;

  virtual int get_successor(int state, const interpretation& symbol) const = 0;
  virtual int get_successor(int state,
                            const interpretation_set& symbol) const = 0;

  virtual bool is_final(int state) const = 0;

  /*!
   * Add a new state.
   *
   * @return the index of the next state.
   */
  virtual int add_state() = 0;

  /*!
   * Set the initial state.
   *
   * @param state the initial state.
   */
  virtual void set_initial_state(int state) = 0;

  /*!
   * Set a state to be final (or not final)
   *
   * @param state the initial state.
   * @param is_final whether the state should be final or not..
   */
  virtual void set_final_state(int state, bool is_final = true) = 0;

  /*!
   * Add a transition to the DFA.
   *
   * The @from and @to parameters must be already existing states
   * of the DFA. The variable indexes (i.e. the keys of @symbols)
   * that are greater than nb_variables are ignored.
   *
   * @param from the starting DFA state
   * @param symbol the guard of the transition. That is, a mapping from indexes
   *    associated to variables, and value of truth of that variable.
   *    Variables indexes missing from the set of keys, or indexes
   *    greater or equal than nb_variables, are interpreted as "don't care".
   * @param to the ending DFA state
   */
  virtual void add_transition(int from, const interpretation_map& symbol,
                              int to) = 0;

  /*!
   * The same the above, but with @symbol as a vector of indexes whose
   * variables are considered true.
   *
   * If @dont_care is true (default), the missing variables are considered
   * "don't care" variables. Otherwise, they are considered explicitly false.
   */
  virtual void add_transition(int from, const interpretation& symbol, int to,
                              bool dont_care = true) = 0;

  /*!
   * The same the above, but with @symbol as a set of indexes whose
   * variables are considered true.
   *
   * If @dont_care is true (default), the missing variables are considered
   * "don't care" variables. Otherwise, they are considered explicitly false.
   */
  virtual void add_transition(int from, const interpretation_set& symbol,
                              int to, bool dont_care = true) = 0;
};
} // namespace whitemech::lydia
