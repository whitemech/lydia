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

#include <cstdio>
#include <graphviz/gvc.h>
#include <lydia/dfa/dfa.hpp>
#include <lydia/utils/misc.hpp>
#include <lydia/utils/print.hpp>
#include <lydia/utils/strings.hpp>
#include <numeric>
#include <queue>

namespace whitemech::lydia {

void dfa_to_graphviz(const abstract_dfa& automaton,
                     const std::string& output_filename,
                     const std::string& format = "svg");

void dfa_to_bdds(const dfa& automaton, const std::string& directory = "./");

/*!
 * Dump the BDDs (one for each bit of the state space).
 *
 * The output will be:
 * output_directory/
 * - 0.dot
 * - 1.dot
 * ...
 * - n.dot
 *
 * That is, a DOT file for each bit.
 *
 * @param directory the directory in which to print the BDDs in DOT format.
 */
void bdd2dot(const dfa& automaton, const std::vector<std::string>& names,
             const std::string& directory);
void dumpdot(const CUDD::Cudd& mgr, const CUDD::BDD& b,
             const std::vector<const char*>& inames,
             const std::string& filename);

} // namespace whitemech::lydia