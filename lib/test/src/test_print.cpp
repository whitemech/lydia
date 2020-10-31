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
#include <catch.hpp>
#include <lydia/utils/print.hpp>

namespace whitemech::lydia::Test {

TEST_CASE("LDLf string printer", "[string_printer]") {

  auto context = AstManager{};
  auto tt = context.makeLdlfTrue();
  auto ff = context.makeLdlfFalse();
  SECTION("test tt.str()") {
    StrPrinter strPrinter;
    auto f = context.makeLdlfTrue();
    auto expected = "tt";
    auto actual = strPrinter.apply(*f);
    REQUIRE(actual == expected);
  }
  SECTION("test ff.str()") {
    auto f = context.makeLdlfFalse();
    auto expected = "ff";
    auto actual = to_string(*f);
    REQUIRE(actual == expected);
  }
  SECTION("to string tt & ff") {
    auto f = context.makeLdlfAnd({tt, ff});
    auto expected = "(tt & ff)";
    auto actual = to_string(*f);
    REQUIRE(actual == expected);
  }
  SECTION("to string tt | ff") {
    auto f = context.makeLdlfOr({tt, ff});
    auto expected = "(tt | ff)";
    auto actual = to_string(*f);
    REQUIRE(actual == expected);
  }
  SECTION("to string !tt") {
    auto f = context.makeLdlfNot(tt);
    auto expected = "!(tt)";
    auto actual = to_string(*f);
    REQUIRE(actual == expected);
  }
  SECTION("to string <a>tt") {
    auto ptr_re =
        std::make_shared<PropositionalRegExp>(context.makePropAtom("a"));
    auto ptr_tt = context.makeLdlfTrue();
    auto f = LDLfDiamond(ptr_re, ptr_tt);
    auto expected = "<a>(tt)";
    auto actual = to_string(f);
    REQUIRE(actual == expected);
  }
  SECTION("to string [a]tt") {
    auto ptr_re =
        std::make_shared<PropositionalRegExp>(context.makePropAtom("a"));
    auto ptr_tt = context.makeLdlfTrue();
    auto f = LDLfBox(ptr_re, ptr_tt);
    auto expected = "[a](tt)";
    auto actual = to_string(f);
    REQUIRE(actual == expected);
  }
}

TEST_CASE("RegEx string printer", "[string_printer]") {
  auto context = AstManager{};
  SECTION("to string a") {
    auto ptr_prop_atom = context.makePropAtom("a");
    auto f = PropositionalRegExp(ptr_prop_atom);
    auto expected = "a";
    auto actual = to_string(f);
    REQUIRE(actual == expected);
  }
  SECTION("to string true") {
    auto ptr_prop_true = context.makeTrue();
    auto prop_re = PropositionalRegExp(ptr_prop_true);
    auto expected = "true";
    auto actual = to_string(prop_re);
    REQUIRE(actual == expected);
  }
  SECTION("to string false") {
    auto ptr_prop_false = context.makeFalse();
    auto prop_re = PropositionalRegExp(ptr_prop_false);
    auto expected = "false";
    auto actual = to_string(prop_re);
    REQUIRE(actual == expected);
  }
  SECTION("to string ?(tt)") {
    auto ptr_ldlf_formula = context.makeLdlfTrue();
    auto f = TestRegExp(ptr_ldlf_formula);
    auto expected = "(tt)?";
    auto actual = to_string(f);
    REQUIRE(actual == expected);
  }
  SECTION("to string (tt & ff)?") {
    auto tt = context.makeLdlfTrue();
    auto ff = context.makeLdlfFalse();
    auto ptr_ldlf_formula = context.makeLdlfAnd(set_formulas({tt, ff}));
    auto f = TestRegExp(ptr_ldlf_formula);
    auto expected = "((tt & ff))?";
    auto actual = to_string(f);
    REQUIRE(actual == expected);
  }
  SECTION("to string a+b") {
    auto a = std::make_shared<PropositionalRegExp>(context.makePropAtom("a"));
    auto b = std::make_shared<PropositionalRegExp>(context.makePropAtom("b"));
    set_regex prop_re = set_regex({a, b});
    auto union_re = UnionRegExp(prop_re);
    auto expected = "(a + b)";
    auto actual = to_string(union_re);
    REQUIRE(actual == expected);
  }
  SECTION("to string a;b") {
    auto a = std::make_shared<PropositionalRegExp>(context.makePropAtom("a"));
    auto b = std::make_shared<PropositionalRegExp>(context.makePropAtom("b"));
    vec_regex prop_re = vec_regex({a, b});
    auto sequence_re = SequenceRegExp(prop_re);
    auto expected = "(a ; b)";
    auto actual = to_string(sequence_re);
    REQUIRE(actual == expected);
  }
  SECTION("to string a*") {
    auto a = std::make_shared<PropositionalRegExp>(context.makePropAtom("a"));
    auto star_re = StarRegExp(a);
    auto expected = "(a)*";
    auto actual = to_string(star_re);
    REQUIRE(actual == expected);
  }
  SECTION("to string (a;b)+(c*)") {
    auto a = std::make_shared<PropositionalRegExp>(context.makePropAtom("a"));
    auto b = std::make_shared<PropositionalRegExp>(context.makePropAtom("b"));
    auto c = std::make_shared<PropositionalRegExp>(context.makePropAtom("c"));
    auto ptr_sequence_re = std::make_shared<SequenceRegExp>(vec_regex({a, b}));
    auto ptr_star_re = std::make_shared<StarRegExp>(c);
    auto union_re = UnionRegExp(set_regex({ptr_sequence_re, ptr_star_re}));
    auto expected = "((a ; b) + (c)*)";
    auto actual = to_string(union_re);
    REQUIRE(actual == expected);
  }
}

TEST_CASE("PropositionalFormula string printer", "[string_printer]") {
  auto context = AstManager{};
  SECTION("to string true") {
    auto prop_true = PropositionalTrue(context);
    auto expected = "true";
    auto actual = to_string(prop_true);
    REQUIRE(actual == expected);
  }
  SECTION("to string false") {
    auto prop_false = PropositionalFalse(context);
    auto expected = "false";
    auto actual = to_string(prop_false);
    REQUIRE(actual == expected);
  }

  auto a = context.makePropAtom("a");
  auto b = context.makePropAtom("b");
  set_prop_formulas set_a_b = set_prop_formulas({a, b});

  SECTION("to string a & b") {
    auto prop_and = context.makePropAnd(set_a_b);
    auto expected = "(a & b)";
    auto actual = to_string(*prop_and);
    REQUIRE(actual == expected);
  }
  SECTION("to string a | b") {
    auto prop_or = context.makePropOr(set_a_b);
    auto expected = "(a | b)";
    auto actual = to_string(*prop_or);
    REQUIRE(actual == expected);
  }
  SECTION("to string !a") {
    auto not_a = context.makePropNot(a);
    auto expected = "!(a)";
    auto actual = to_string(*not_a);
    REQUIRE(actual == expected);
  }
}

} // namespace whitemech::lydia::Test