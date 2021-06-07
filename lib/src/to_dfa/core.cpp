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

#include <lydia/dfa/dfa.hpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/strategies/bdd/base.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>
#include <lydia/to_dfa/strategies/naive.hpp>
#include <memory>

namespace whitemech::lydia {

std::shared_ptr<abstract_dfa> to_dfa_with_strategy(const LDLfFormula& formula,
                                                   Strategy& s) {
  auto t = Translator(s);
  return t.to_dfa(formula);
}

std::shared_ptr<abstract_dfa> to_dfa(const LDLfFormula& formula) {
  auto s = CompositionalStrategy();
  return to_dfa_with_strategy(formula, s);
}

} // namespace whitemech::lydia