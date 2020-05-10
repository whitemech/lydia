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

#include <cuddObj.hh>
#include <lydia/dfa.hpp>
#include <lydia/logic.hpp>
#include <lydia/parser/driver.hpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/utils/dfa_transform.hpp>
#include <sstream>

namespace whitemech {
namespace lydia {

static dfa_ptr to_dfa_from_formula_string(const std::string &f,
                                          const CUDD::Cudd &mgr) {
  auto driver = Driver();
  std::stringstream ldlf_formula_stream(f);
  driver.parse(ldlf_formula_stream);
  const auto &formula = *driver.result;
  return to_dfa(formula, mgr);
}

static void print_dfa(const dfa &automaton, const std::string &name,
                      const std::string &format = "svg") {
  dfa_to_graphviz(automaton, name + format, format);
}

} // namespace lydia
} // namespace whitemech