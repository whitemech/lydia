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

#include <lydia/dfa/mona_dfa.hpp>

namespace whitemech {
namespace lydia {

mona_dfa::mona_dfa(DFA *dfa) : dfa_{dfa} {}
int mona_dfa::get_nb_states() const { return dfa_->ns; }
int mona_dfa::get_initial_state() const { return dfa_->s; }
int mona_dfa::get_nb_variables() const { return dfa_->s; }

} // namespace lydia
} // namespace whitemech