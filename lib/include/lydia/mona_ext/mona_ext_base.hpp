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

#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

extern "C" {
#include <mona/bdd.h>
#include <mona/dfa.h>
#include <mona/hash.h>
#include <mona/mem.h>
}

DFA* dfaUniversalProject(DFA* a, unsigned var_index);

namespace whitemech::lydia {

DFA* dfa_concatenate(DFA* a, DFA* b, int n, int* indices);

DFA* dfa_closure(DFA* a, int n, int* indices);

DFA* only_empty();

DFA* dfa_accept_empty(DFA* x);

std::string get_path_guard(int n, trace_descr tp);

DFA* dfaLDLfTrue();

DFA* dfaLDLfFalse();

DFA* dfaNext(int a, bool is_positive = true);

DFA* dfaLDLfDiamondProp(DFA* prop_regex, DFA* body, int var, int* indices);

DFA* dfaLDLfEnd(int var, int* indices);

DFA* dfaPropositionalTrue();

bool is_sink(DFA* automaton, bool is_positive = true);

void print_mona_dfa(DFA* a, const std::string& name, int num = 1);

void dfaPrintGraphvizToFile(DFA* a, int no_free_vars, unsigned* offsets,
                            std::ostream& o = std::cout);

void dfaPrint(DFA* a, int no_free_vars, std::vector<std::string> free_variables,
              unsigned* offsets, std::ostream& o = std::cout);

} // namespace whitemech::lydia