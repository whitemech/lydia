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
#include <catch.hpp>
#include <lydia/logic/pl/base.hpp>
#include <lydia/logic/pl/cnf.hpp>
#include <lydia/logic/pl/eval.hpp>
#include <lydia/logic/pl/models/base.hpp>
#include <lydia/logic/pl/models/naive.hpp>

namespace whitemech::lydia::Test {
TEST_CASE("Propositional Logic", "[pl/logic]") {

  auto context = AstManager();
  auto t = context.makeTrue();
  auto f = context.makeFalse();
  auto a = context.makePropAtom("a");
  auto b = context.makePropAtom("b");
  auto a_and_b = context.makePropAnd(set_prop_formulas{a, b});
  auto a_or_b = context.makePropOr(set_prop_formulas{a, b});

  auto empty = set_atoms_ptr();
  auto i_a = set_atoms_ptr{a};
  auto i_b = set_atoms_ptr{b};
  auto i_ab = set_atoms_ptr{a, b};

  REQUIRE(!eval(*a_and_b, empty));
  REQUIRE(!eval(*a_and_b, i_a));
  REQUIRE(!eval(*a_and_b, i_b));
  REQUIRE(eval(*a_and_b, i_ab));

  REQUIRE(!eval(*a_or_b, empty));
  REQUIRE(eval(*a_or_b, i_a));
  REQUIRE(eval(*a_or_b, i_b));
  REQUIRE(eval(*a_or_b, i_ab));
}

TEST_CASE("Simple logical operation", "[pl/logic]") {
  auto context = AstManager{};
  auto t = context.makeTrue();
  auto f = context.makeFalse();

  SECTION("true & false = false") {
    auto t_and_f = context.makePropAnd({t, f});
    REQUIRE(*t_and_f == *f);
  }

  SECTION("true | false = true") {
    auto t_or_f = context.makePropOr({t, f});
    REQUIRE(t_or_f == t);
  }

  SECTION("and(p) = p") {
    auto p = context.makePropAtom("p");
    auto and_p = context.makePropAnd({p});
    REQUIRE(and_p == p);
  }

  //  TODO reintroduce this simplification
  //  SECTION("p & ~p = false") {
  //    auto p = context.makePropAtom("p");
  //    auto not_p = p->logical_not();
  //    auto and_p = context.makePropAnd({p, not_p});
  //    REQUIRE(and_p == f);
  //  }
  //
  //  SECTION("p | ~p = true") {
  //    auto p = context.makePropAtom("p");
  //    auto not_p = p->logical_not();
  //    auto or_p = context.makePropOr({p, not_p});
  //    REQUIRE(or_p == t);
  //  }

  SECTION("~(p & q) = ~p | ~q") {
    auto p = context.makePropAtom("p");
    auto q = context.makePropAtom("q");
    auto not_p = p->logical_not();
    auto not_q = q->logical_not();
    auto and_pq = context.makePropAnd({p, q});
    auto expected = context.makePropOr({not_p, not_q});
    REQUIRE(and_pq->logical_not()->is_equal(*expected));
  }
}

TEST_CASE("Logical and", "[logic]") {
  auto context = AstManager{};
  auto ptr_a = context.makePropAtom("a");
  auto ptr_b = context.makePropAtom("b");
  auto ptr_c = context.makePropAtom("c");
  auto ptr_true = context.makeTrue();
  auto ptr_false = context.makeFalse();

  set_prop_formulas a_b = set_prop_formulas({ptr_a, ptr_b});
  auto ptr_and_a_b = context.makePropAnd(a_b);
  set_prop_formulas b_c = set_prop_formulas({ptr_b, ptr_c});
  auto ptr_and_b_c = context.makePropAnd(b_c);

  SECTION("(a & b) & c == a & b & c") {
    set_prop_formulas and_ab_c = set_prop_formulas({ptr_and_a_b, ptr_c});
    auto ptr_and_ab_c = context.makePropAnd(and_ab_c);
    set_prop_formulas exp_and = set_prop_formulas({ptr_a, ptr_b, ptr_c});
    auto exp_flat_and = context.makePropAnd(exp_and);
    auto flattened_and = context.makePropAnd(ptr_and_ab_c->get_container());
    REQUIRE(*flattened_and == *exp_flat_and);
  }
  SECTION("a & (b & c) == a & b & c") {
    set_prop_formulas and_a_bc = set_prop_formulas({ptr_a, ptr_and_b_c});
    auto ptr_and_a_bc = context.makePropAnd(and_a_bc);
    set_prop_formulas exp_and = set_prop_formulas({ptr_a, ptr_b, ptr_c});
    auto exp_flat_and = context.makePropAnd(exp_and);
    auto flattened_and = context.makePropAnd(ptr_and_a_bc->get_container());
    REQUIRE(*flattened_and == *exp_flat_and);
  }
  SECTION("a & b & false == false") {
    set_prop_formulas and_a_b_false =
        set_prop_formulas({ptr_a, ptr_b, ptr_false});
    auto ptr_and_a_b_false = context.makePropAnd(and_a_b_false);
    REQUIRE(*ptr_and_a_b_false == *ptr_false);
  }
  SECTION("a & true & c == a & c") {
    set_prop_formulas and_a_true_c =
        set_prop_formulas({ptr_a, ptr_true, ptr_c});
    auto ptr_and_a_true_c = context.makePropAnd(and_a_true_c);
    set_prop_formulas exp_and = set_prop_formulas({ptr_a, ptr_c});
    auto exp_flat_and = context.makePropAnd(exp_and);
    auto flattened_and = context.makePropAnd(ptr_and_a_true_c->get_container());
    REQUIRE(*flattened_and == *exp_flat_and);
  }
}

TEST_CASE("Logical or", "[logic]") {
  auto context = AstManager{};
  auto ptr_a = context.makePropAtom("a");
  auto ptr_b = context.makePropAtom("b");
  auto ptr_c = context.makePropAtom("c");
  auto ptr_true = context.makeTrue();
  auto ptr_false = context.makeFalse();

  set_prop_formulas a_b = set_prop_formulas({ptr_a, ptr_b});
  auto ptr_or_a_b = context.makePropOr(a_b);
  set_prop_formulas b_c = set_prop_formulas({ptr_b, ptr_c});
  auto ptr_or_b_c = context.makePropOr(b_c);

  SECTION("(a | b) | c == a | b | c") {
    set_prop_formulas or_ab_c = set_prop_formulas({ptr_or_a_b, ptr_c});
    auto ptr_or_ab_c = context.makePropOr(or_ab_c);
    set_prop_formulas exp_or = set_prop_formulas({ptr_a, ptr_b, ptr_c});
    auto exp_flat_or = context.makePropOr(exp_or);
    auto flattened_or = context.makePropOr(ptr_or_ab_c->get_container());
    REQUIRE(*flattened_or == *exp_flat_or);
  }
  SECTION("a | (b | c) == a | b | c") {
    set_prop_formulas or_a_bc = set_prop_formulas({ptr_a, ptr_or_b_c});
    auto ptr_or_a_bc = context.makePropOr(or_a_bc);
    set_prop_formulas exp_or = set_prop_formulas({ptr_a, ptr_b, ptr_c});
    auto exp_flat_or = context.makePropOr(exp_or);
    auto flattened_or = context.makePropOr(ptr_or_a_bc->get_container());
    REQUIRE(*flattened_or == *exp_flat_or);
  }
  SECTION("a | b | false == a | b") {
    set_prop_formulas or_a_b_false =
        set_prop_formulas({ptr_a, ptr_b, ptr_false});
    auto ptr_or_a_b_false = context.makePropOr(or_a_b_false);
    set_prop_formulas exp_or = set_prop_formulas({ptr_a, ptr_b});
    auto exp_flat_or = context.makePropOr(exp_or);
    auto flattened_or = context.makePropOr(ptr_or_a_b_false->get_container());
    REQUIRE(*flattened_or == *exp_flat_or);
  }
  SECTION("a | true | c == true") {
    set_prop_formulas or_a_true_c = set_prop_formulas({ptr_a, ptr_true, ptr_c});
    auto ptr_or_a_true_c = context.makePropOr(or_a_true_c);
    REQUIRE(*ptr_or_a_true_c == *ptr_true);
  }
}

TEST_CASE("to cnf", "[pl/cnf]") {
  auto context = AstManager{};

  SECTION("True is in CNF") {
    auto t = context.makeTrue();
    auto actual = to_cnf(*t);
    REQUIRE(*t == *actual);
  }

  SECTION("False is in CNF") {
    auto f = context.makeFalse();
    auto actual = to_cnf(*f);
    REQUIRE(*f == *actual);
  }

  SECTION("Propositional is in CNF") {
    auto p = context.makePropAtom("p");
    auto actual = to_cnf(*p);
    REQUIRE(*actual == *p);
  }
  SECTION("Not of atom is in CNF") {
    auto p = context.makePropAtom("p");
    auto not_p = p->logical_not();
    auto actual = to_cnf(*not_p);
    REQUIRE(*actual == *not_p);
  }
  SECTION("Or of atoms is in CNF") {
    auto p = context.makePropAtom("p");
    auto q = context.makePropAtom("q");
    auto expected = context.makePropOr({p, q});
    auto actual = to_cnf(*expected);
    REQUIRE(*actual == *expected);
  }
  SECTION("And of atoms is in CNF") {
    auto p = context.makePropAtom("p");
    auto q = context.makePropAtom("q");
    auto expected = context.makePropAnd({p, q});
    auto actual = to_cnf(*expected);
    REQUIRE(*actual == *expected);
  }
  SECTION("Not of And") {
    auto p = context.makePropAtom("p");
    auto q = context.makePropAtom("q");
    auto f = context.makePropAnd({p, q})->logical_not();
    auto expected = context.makePropOr({p->logical_not(), q->logical_not()});
    auto actual = to_cnf(*f);
    REQUIRE(*actual == *expected);
  }
  SECTION("Not of Or") {
    auto p = context.makePropAtom("p");
    auto q = context.makePropAtom("q");
    auto f = context.makePropOr({p, q})->logical_not();
    auto expected = context.makePropAnd({p->logical_not(), q->logical_not()});
    auto actual = to_cnf(*f);
    REQUIRE(*actual == *expected);
  }
  SECTION("Or with one And") {
    auto p = context.makePropAtom("p");
    auto q = context.makePropAtom("q");
    auto r = context.makePropAtom("r");
    auto and_pq = context.makePropAnd({p, q});
    auto or_r = context.makePropOr({r, and_pq});
    auto expected = context.makePropAnd({
        context.makePropOr({r, p}),
        context.makePropOr({r, q}),
    });
    auto actual = to_cnf(*or_r);
    REQUIRE(*actual == *expected);
  }
  SECTION("Or with two Ands") {
    // (p & q) | (r & s)
    // expected: (p | r) & (p | s) & (q | r) & (q | s)
    auto p = context.makePropAtom("p");
    auto q = context.makePropAtom("q");
    auto r = context.makePropAtom("r");
    auto s = context.makePropAtom("s");
    auto and_pq = context.makePropAnd({p, q});
    auto and_rs = context.makePropAnd({r, s});
    auto or_rs_pq = context.makePropOr({and_rs, and_pq});
    auto expected = context.makePropAnd({
        context.makePropOr({r, p}),
        context.makePropOr({r, q}),
        context.makePropOr({s, p}),
        context.makePropOr({s, q}),
    });
    auto actual = to_cnf(*or_rs_pq);
    REQUIRE(*actual == *expected);
  }
  SECTION("And of Ors is in CNF") {
    auto p = context.makePropAtom("p");
    auto q = context.makePropAtom("q");
    auto r = context.makePropAtom("r");
    auto s = context.makePropAtom("s");
    auto or_pq = context.makePropOr({p, q});
    auto or_rs = context.makePropOr({r, s});
    auto and_rs_pq = context.makePropOr({or_rs, or_pq});
    auto actual = to_cnf(*and_rs_pq);
    REQUIRE(*actual == *and_rs_pq);
  }
}

struct SetComparator {
  template <typename T, typename U>
  bool operator()(const std::set<T, U>& lhs, const std::set<T, U>& rhs) const {
    return unified_compare(lhs, rhs) == -1;
  }
};

TEST_CASE("All models", "[pl/models]") {
  auto context = AstManager{};
  auto p = context.makePropAtom("p");
  auto q = context.makePropAtom("q");
  auto r = context.makePropAtom("r");
  auto s = context.makePropAtom("s");
  auto true_ = context.makeTrue();
  auto false_ = context.makeFalse();

  auto model_enumeration_function =
      GENERATE(all_models<NaiveModelEnumerationStategy>);

  SECTION("models of true") {
    auto models = model_enumeration_function(*true_);
    REQUIRE(models.size() == 1);
    REQUIRE(models[0].empty());
  }
  SECTION("models of false") {
    auto models = model_enumeration_function(*false_);
    REQUIRE(models.empty());
  }

  SECTION("models of p") {
    auto models = model_enumeration_function(*p);
    REQUIRE(models.size() == 1);
    auto expected_model = set_atoms_ptr({p});
    auto actual_model = models[0];
    REQUIRE(unified_eq(actual_model, expected_model));
  }
  SECTION("models of ~q") {
    auto models = model_enumeration_function(*q->logical_not());
    REQUIRE(models.size() == 1);
    REQUIRE(models[0].empty());
  }

  SECTION("models of p | q") {
    auto models = model_enumeration_function(*context.makePropOr({p, q}));
    REQUIRE(models.size() == 3);
    auto expected_models = std::set<set_atoms_ptr, SetComparator>({
        set_atoms_ptr({p, q}),
        set_atoms_ptr({p}),
        set_atoms_ptr({q}),
    });
    auto actual_models =
        std::set<set_atoms_ptr, SetComparator>(models.begin(), models.end());
    REQUIRE(unified_eq(expected_models, actual_models));
  }
  SECTION("models of p & q") {
    auto models = model_enumeration_function(*context.makePropAnd({p, q}));
    REQUIRE(models.size() == 1);
    REQUIRE(unified_eq(models[0], set_atoms_ptr({p, q})));
  }
  SECTION("models of !(p | q)") {
    auto models =
        model_enumeration_function(*context.makePropOr({p, q})->logical_not());
    REQUIRE(models.size() == 1);
    REQUIRE(unified_eq(models[0], set_atoms_ptr({})));
  }
  SECTION("models of r & (p | q)") {
    auto f = context.makePropAnd({r, context.makePropOr({p, q})});
    auto models = model_enumeration_function(*f);
    REQUIRE(models.size() == 3);
    auto expected_models = std::set<set_atoms_ptr, SetComparator>({
        set_atoms_ptr({r, p}),
        set_atoms_ptr({r, q}),
        set_atoms_ptr({r, p, q}),
    });
    auto actual_models =
        std::set<set_atoms_ptr, SetComparator>(models.begin(), models.end());
    REQUIRE(unified_eq(actual_models, expected_models));
  }

  SECTION("models of (r | !s) & (!r | !q)") {
    auto f = context.makePropAnd({
        context.makePropOr({r, s->logical_not()}),
        context.makePropOr({q->logical_not(), r->logical_not()}),
    });
    auto models = model_enumeration_function(*f);
    REQUIRE(models.size() == 4);
  }
}

} // namespace whitemech::lydia::Test