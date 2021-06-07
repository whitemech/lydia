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

#include "to_dfa.hpp"

namespace whitemech::lydia {

// via GeneratorUntypedBase:
// Attempts to move the generator to the next element.
// Returns true if successful (and thus has another element that can be read)
bool StrategyGenerator::next() { return ++i < factories.size(); }
// Precondition:
// The generator is either freshly constructed or the last call to next()
// returned true
std::function<std::shared_ptr<Strategy>(const CUDD::Cudd&)> const&
StrategyGenerator::get() const {
  return factories.at(i);
}

StrategyGenerator::StrategyGenerator() {
  factories =
      std::vector<std::function<std::shared_ptr<Strategy>(const CUDD::Cudd&)>>({
          StrategyGenerator::make_bdd,
          StrategyGenerator::make_compositional,
      });
}

Catch::Generators::GeneratorWrapper<
    std::function<std::shared_ptr<Strategy>(const CUDD::Cudd&)>>
strategies() {
  return Catch::Generators::GeneratorWrapper<
      std::function<std::shared_ptr<Strategy>(const CUDD::Cudd&)>>(
      std::make_unique<StrategyGenerator>());
}

} // namespace whitemech::lydia