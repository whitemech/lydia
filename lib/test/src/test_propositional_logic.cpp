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
#include <cryptominisat5/cryptominisat.h>
#include <pl/cnf.hpp>
#include <pl/eval.hpp>
#include <pl/logic.hpp>
#include <pl/models_sat.hpp>

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

  SECTION("~(p & q) = ~p | ~q") {
    auto p = prop_atom("p");
    auto q = prop_atom("q");
    auto not_p = p->logical_not();
    auto not_q = q->logical_not();
    auto and_pq = logical_and({p, q});
    auto expected = logical_or({not_p, not_q});
    REQUIRE(and_pq->logical_not()->is_equal(*expected));
  }
}

TEST_CASE("to cnf", "[pl/cnf]") {

  SECTION("True is in CNF") {
    auto t = boolean_prop(true);
    auto actual = to_cnf(*t);
    REQUIRE(*t == *actual);
  }

  SECTION("False is in CNF") {
    auto f = boolean_prop(false);
    auto actual = to_cnf(*f);
    REQUIRE(*f == *actual);
  }

  SECTION("Propositional is in CNF") {
    auto p = prop_atom("p");
    auto actual = to_cnf(*p);
    REQUIRE(*actual == *p);
  }
  SECTION("Not of atom is in CNF") {
    auto p = prop_atom("p");
    auto not_p = p->logical_not();
    auto actual = to_cnf(*not_p);
    REQUIRE(*actual == *not_p);
  }
  SECTION("Or of atoms is in CNF") {
    auto p = prop_atom("p");
    auto q = prop_atom("q");
    auto expected = PropositionalOr({p, q});
    auto actual = to_cnf(expected);
    REQUIRE(*actual == expected);
  }
  SECTION("And of atoms is in CNF") {
    auto p = prop_atom("p");
    auto q = prop_atom("q");
    auto expected = PropositionalAnd({p, q});
    auto actual = to_cnf(expected);
    REQUIRE(*actual == expected);
  }
  SECTION("Or with one And") {
    auto p = prop_atom("p");
    auto q = prop_atom("q");
    auto r = prop_atom("r");
    auto and_pq = logical_and({p, q});
    auto or_r = logical_or({r, and_pq});
    auto expected = logical_and({
        logical_or({r, p}),
        logical_or({r, q}),
    });
    auto actual = to_cnf(*or_r);
    REQUIRE(*actual == *expected);
  }
  SECTION("Or with two Ands") {
    // (p & q) | (r & s)
    // expected: (p | r) & (p | s) & (q | r) & (q | s)
    auto p = prop_atom("p");
    auto q = prop_atom("q");
    auto r = prop_atom("r");
    auto s = prop_atom("s");
    auto and_pq = logical_and({p, q});
    auto and_rs = logical_and({r, s});
    auto or_rs_pq = logical_or({and_rs, and_pq});
    auto expected = logical_and({
        logical_or({r, p}),
        logical_or({r, q}),
        logical_or({s, p}),
        logical_or({s, q}),
    });
    auto actual = to_cnf(*or_rs_pq);
    REQUIRE(*actual == *expected);
  }
  SECTION("And of Ors is in CNF") {
    auto p = prop_atom("p");
    auto q = prop_atom("q");
    auto r = prop_atom("r");
    auto s = prop_atom("s");
    auto or_pq = logical_or({p, q});
    auto or_rs = logical_or({r, s});
    auto and_rs_pq = logical_or({or_rs, or_pq});
    auto actual = to_cnf(*and_rs_pq);
    REQUIRE(*actual == *and_rs_pq);
  }
}

TEST_CASE("Test Cryptominisat", "[cryptominisat]") {
  CMSat::SATSolver solver;
  std::vector<CMSat::Lit> clause;

  // Let's use 4 threads
  solver.set_num_threads(4);

  // We need 3 variables. They will be: 0,1,2
  // Variable numbers are always trivially increasing
  solver.new_vars(3);

  // add "1 0"
  clause.emplace_back(0, false);
  solver.add_clause(clause);

  // add "-2 0"
  clause.clear();
  clause.emplace_back(1, true);
  solver.add_clause(clause);

  // add "-1 2 3 0"
  clause.clear();
  clause.emplace_back(0, true);
  clause.emplace_back(1, false);
  clause.emplace_back(2, false);
  solver.add_clause(clause);

  CMSat::lbool ret = solver.solve();
  REQUIRE(ret == CMSat::l_True);
  std::cout << "Solution is: " << solver.get_model()[0] << ", "
            << solver.get_model()[1] << ", " << solver.get_model()[2]
            << std::endl;

  // assumes 3 = FALSE, no solutions left
  std::vector<CMSat::Lit> assumptions;
  assumptions.emplace_back(2, true);
  ret = solver.solve(&assumptions);
  REQUIRE(ret == CMSat::l_False);

  // without assumptions we still have a solution
  ret = solver.solve();
  REQUIRE(ret == CMSat::l_True);

  // add "-3 0"
  // No solutions left, UNSATISFIABLE returned
  clause.clear();
  clause.emplace_back(2, true);
  solver.add_clause(clause);
  ret = solver.solve();
  REQUIRE(ret == CMSat::l_False);
}

TEST_CASE("is_sat", "[pl/models]") {
  //  auto t = boolean_prop(true);
  //  auto f = boolean_prop(false);
  auto p = prop_atom("p");
  auto q = prop_atom("q");
  REQUIRE(is_sat(*p));
  REQUIRE(is_sat(*q));
  REQUIRE(is_sat(*logical_or({p, q})));
  REQUIRE(is_sat(*logical_and({p, q})));

  auto non_sat =
      logical_and({logical_or({p, q}), p->logical_not(), q->logical_not()});
  REQUIRE(!is_sat(*non_sat));
}

TEST_CASE("All models", "[pl/models]") {
  auto p = prop_atom("p");
  auto q = prop_atom("q");

  SECTION("models of p") {
    auto models = all_models_sat(*p);
    REQUIRE(models.size() == 1);
    REQUIRE(models[0] == set_atoms_ptr{p});
  }
  SECTION("models of ~q") {
    auto models = all_models_sat(*p->logical_not());
    REQUIRE(models.size() == 1);
    REQUIRE(models[0].empty());
  }

  SECTION("models of p | q") {
    auto models = all_models_sat(*logical_or({p, q}));
    REQUIRE(models.size() == 3);
    auto expected_models = std::set<set_atoms_ptr>({
        set_atoms_ptr({p, q}),
        set_atoms_ptr({p}),
        set_atoms_ptr({q}),
    });
    auto actual_models = std::set<set_atoms_ptr>(models.begin(), models.end());
    REQUIRE(expected_models == actual_models);
  }
  SECTION("models of p & q") {
    auto models = all_models_sat(*logical_and({p, q}));
    REQUIRE(models.size() == 1);
    REQUIRE(models[0] == set_atoms_ptr({p, q}));
  }
  SECTION("models of !(p | q)") {
    auto models = all_models_sat(*logical_or({p, q})->logical_not());
    REQUIRE(models.size() == 1);
    REQUIRE(models[0] == set_atoms_ptr({}));
  }
}

} // namespace whitemech::lydia::Test