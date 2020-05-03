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
#include "catch.hpp"
#include <pl/eval.hpp>
#include <pl/logic.hpp>

namespace whitemech::lydia::Test {
TEST_CASE("Propositional Logic", "[pl/logic]") {

  auto t = std::make_shared<PropositionalTrue>();
  auto f = std::make_shared<PropositionalFalse>();
  auto a = std::make_shared<PropositionalAtom>("a");
  auto b = std::make_shared<PropositionalAtom>("b");
  auto a_and_b = PropositionalAnd(set_prop_formulas{a, b});
  auto a_or_b = PropositionalOr(set_prop_formulas{a, b});

  auto empty = set_atoms_ptr();
  auto i_a = set_atoms_ptr{a};
  auto i_b = set_atoms_ptr{b};
  auto i_ab = set_atoms_ptr{a, b};

  REQUIRE(!eval(a_and_b, empty));
  REQUIRE(!eval(a_and_b, i_a));
  REQUIRE(!eval(a_and_b, i_b));
  REQUIRE(eval(a_and_b, i_ab));

  REQUIRE(!eval(a_or_b, empty));
  REQUIRE(eval(a_or_b, i_a));
  REQUIRE(eval(a_or_b, i_b));
  REQUIRE(eval(a_or_b, i_ab));
}

TEST_CASE("Logical operation", "[pl/logic]") {
  auto t = boolean_prop(true);
  auto f = boolean_prop(false);

  SECTION("true & false = false") {
    auto t_and_f = logical_and({t, f});
    REQUIRE(t_and_f == f);
  }

  SECTION("true | false = true") {
    auto t_or_f = logical_or({t, f});
    REQUIRE(t_or_f == t);
  }

  SECTION("and(p) = p") {
    auto p = prop_atom("p");
    auto and_p = logical_and({p});
    REQUIRE(and_p == p);
  }

  SECTION("p & ~p = false") {
    auto p = prop_atom("p");
    auto not_p = p->logical_not();
    auto and_p = logical_and({p, not_p});
    REQUIRE(and_p == f);
  }

  SECTION("p | ~p = true") {
    auto p = prop_atom("p");
    auto not_p = p->logical_not();
    auto or_p = logical_or({p, not_p});
    REQUIRE(or_p == t);
  }
}

} // namespace whitemech::lydia::Test