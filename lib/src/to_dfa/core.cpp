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

#include <lydia/dfa/dfa.hpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/strategies/bdd/base.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>
#include <lydia/to_dfa/strategies/naive.hpp>
#include <lydia/to_dfa/strategies/sat.hpp>
#include <memory>

namespace whitemech {
namespace lydia {

std::shared_ptr<abstract_dfa> to_dfa(const LDLfFormula &formula) {
  //    auto s = NaiveStrategy(mgr);
  //  auto s = SATStrategy(mgr);
  //  auto s = BDDStrategy(mgr);
  auto s = CompositionalStrategy();
  auto t = Translator(s);
  return t.to_dfa(formula);
}
} // namespace lydia
} // namespace whitemech