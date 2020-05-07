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

#include <lydia/dfa.hpp>
#include <lydia/logic.hpp>
#include <lydia/types.hpp>
#include <lydia/utils/compare.hpp>
#include <memory>
#include <utility>

namespace whitemech {
namespace lydia {

/*!
 *
 * Translate an LDLf formula into an DFA.
 *
 * This is one of the main procedures of the library.
 *
 * @param formula the LDLf formula.
 * @return the equivalent DFA.
 */
std::shared_ptr<dfa> to_dfa(const LDLfFormula &formula, const CUDD::Cudd &mgr);

std::shared_ptr<dfa> to_dfa_sat(const LDLfFormula &formula,
                                const CUDD::Cudd &mgr);

} // namespace lydia
} // namespace whitemech
