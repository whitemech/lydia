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

#include <lydia/dfa/mona_dfa.hpp>

namespace whitemech::lydia {

int mona_dfa::get_nb_states() const { return dfa_->ns; }

int mona_dfa::get_initial_state() const { return dfa_->s; }

int mona_dfa::get_nb_variables() const { return nb_variables_; }

mona_dfa::~mona_dfa() { dfaFree(this->dfa_); }

bool mona_dfa::is_final(int state) const { return dfa_->f[state] == 1; }

int mona_dfa::get_successor(int state, const interpretation& symbol) const {
  bdd_manager* mgr = this->dfa_->bddm;
  unsigned l, r, index;
  unsigned current_node = dfa_->q[state];
  LOAD_lri(&mgr->node_table[current_node], l, r,
           index) while (index != BDD_LEAF_INDEX) {
    current_node = symbol[index] ? r : l;
    LOAD_lri(&mgr->node_table[current_node], l, r, index)
  }
  return l;
}

void mona_dfa::export_dfa(const std::string& filename) const {
  std::vector<char> filename_cstr(filename.c_str(),
                                  filename.c_str() + filename.size() + 1);
  char** arr = new char*[names.size()];
  for (size_t i = 0; i < names.size(); i++) {
    arr[i] = new char[names[i].size() + 1];
    strcpy(arr[i], names[i].c_str());
  }
  char* orders = new char[names.size()];
  memset(orders, 2, names.size());

  dfaExport(dfa_, filename_cstr.data(), get_nb_variables(), arr, orders);
  for (size_t i = 0; i < names.size(); i++) {
    delete[] arr[i];
  }
  delete[] arr;
}

} // namespace whitemech::lydia