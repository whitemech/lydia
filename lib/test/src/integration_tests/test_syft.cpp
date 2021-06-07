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

#include "test/src/utils/to_dfa.hpp"
#include <catch.hpp>
#include <filesystem>
#include <lydia/dfa/mona_dfa.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>
#include <mona/bdd.h>

namespace whitemech::lydia {

static Logger log("dataset-ltlf_1");
static const auto ROOT = std::filesystem::path("..") / ".." / "..";
static const auto DATASETS_ROOT =
    ROOT / "lib" / "test" / "src" / "assets" / "datasets";

int find_sink(DFA* M) {
  int i;
  for (i = 0; i < M->ns; i++) {
    if (bdd_is_leaf(M->bddm, M->q[i]) &&
        (bdd_leaf_value(M->bddm, M->q[i]) == i) && (M->f[i] == -1))
      return i;
  }
  return -1;
}

static void visit_leaves(bdd_manager* bddm, unsigned p) {
  bool is_leaf = bdd_is_leaf(bddm, p);
  bool is_marked = bdd_mark(bddm, p);
  if (is_leaf and not is_marked) {
    bdd_set_mark(bddm, p, true);
    unsigned old_left, old_right;
    unsigned new_first, actual_new_left, actual_new_right;
    unsigned old_leaf_value = bdd_leaf_value(bddm, p);
    bdd_record* leaf = &bddm->node_table[p];
    LOAD_lr(leaf, old_left, old_right) new_first = old_leaf_value - 1;
    new_first = new_first << 8;
    new_first |= (old_right >> 16) & 0xff;
    leaf->lri[0] = new_first;

    LOAD_lr(leaf, actual_new_left, actual_new_right)
        assert(actual_new_left == old_left - 1);
    assert(actual_new_right == old_right);
  } else if (not is_marked) {
    bdd_set_mark(bddm, p, true);
    visit_leaves(bddm, bdd_then(bddm, p));
    visit_leaves(bddm, bdd_else(bddm, p));
  }
}

DFA* remove_zero_state(DFA* automaton) {
  // TODO free memory of BDD nodes from zero state!
  DFA* tmp = dfaCopy(automaton);
  if (tmp->ns == 1) {
    return tmp;
  }
  int old_ns = tmp->ns;
  tmp->ns = tmp->ns - 1;
  tmp->s = 0;

  for (int i = 0; i < old_ns - 1; i++) {
    tmp->f[i] = tmp->f[i + 1];
    tmp->q[i] = tmp->q[i + 1];
  }

  bdd_prepare_apply1(tmp->bddm);
  for (int i = 0; i < old_ns - 1; i++) {
    visit_leaves(tmp->bddm, tmp->q[i]);
  }

  return tmp;
}

std::shared_ptr<abstract_dfa> load_mona_dfa(std::filesystem::path filepath) {
  char** names;
  int* orders;
  int index;
  std::string filename = filepath.filename().string();
  std::string output = "output.dfa";
  const std::string command("mona -q -u -xw " + filepath.string() + " > " +
                            output);
  log.info("MONA: Executing command: '" + command + "'");
  int ret = std::system(command.data());
  if (ret > 0) {
    log.error("Command failed: " + command);
  }
  auto dfa = dfaImport(output.data(), &names, &orders);
  std::vector<std::string> names_vector;
  for (index = 0; names[index]; index++) {
    names_vector.push_back(names[index]);
  }
  return std::make_shared<mona_dfa>(dfa, names_vector);
}

static void _dataset_test(const std::filesystem::path& dataset_path) {
  log.info("Processing dataset " + dataset_path.string());
  const auto mona_dataset = dataset_path / "mona";
  const auto lydia_dataset = dataset_path / "lydia";

  std::filesystem::directory_iterator it(mona_dataset);
  for (const auto& datapath : it) {
    log.info("Processing file " + dataset_path.string());
    const auto strategy = std::make_shared<CompositionalStrategy>();
    const auto filename = datapath.path().filename().stem().string();
    const auto mona_file = mona_dataset / fmt::format("{}.mona", filename);
    const auto lydia_file = lydia_dataset / fmt::format("{}.ldlf", filename);
    DFA* prop_true = dfaPropositionalTrue();
    DFA* ldlf_true = dfaLDLfTrue();
    DFA* at_least_one_step =
        dfaLDLfDiamondProp(prop_true, ldlf_true, 0, nullptr);
    dfaFree(prop_true);
    dfaFree(ldlf_true);

    // Compute MONA DFA
    auto temp_mona_dfa =
        std::static_pointer_cast<mona_dfa>(load_mona_dfa(mona_file));
    auto expected_mona_dfa = remove_zero_state(temp_mona_dfa->dfa_);
    auto expected_dfa =
        std::make_shared<mona_dfa>(expected_mona_dfa, temp_mona_dfa->names);

    // Compute Lydia Dfa
    // add "[true]end" in AND
    log.info("Lydia: Computing automaton " + lydia_file.string());
    auto temp_lydia_dfa = std::static_pointer_cast<mona_dfa>(
        to_dfa_from_formula_file(lydia_file, *strategy));
    auto actual_lydia_dfa =
        dfaProduct(temp_lydia_dfa->dfa_, at_least_one_step, dfaAND);
    actual_lydia_dfa = dfaMinimize(actual_lydia_dfa);
    auto actual_dfa =
        std::make_shared<mona_dfa>(actual_lydia_dfa, temp_lydia_dfa->names);
    dfaFree(at_least_one_step);
    int expected_nb_vars = expected_dfa->get_nb_variables();
    int actual_nb_vars = actual_dfa->get_nb_variables();
    REQUIRE(expected_nb_vars == actual_nb_vars);
    //  commented because of the additional 'zero' state in mona.
    int expected_nb_states = expected_dfa->get_nb_states();
    int actual_nb_states = actual_dfa->get_nb_states();
    REQUIRE((expected_nb_states == actual_nb_states));
    REQUIRE(compare<1>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<2>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<3>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<4>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<5>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<6>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<7>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<8>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<9>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<10>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));

    // compare shortest positive/negative examples
    char *expected_example, *actual_example;
    expected_example =
        dfaMakeExample(expected_dfa->dfa_, 1, expected_dfa->get_nb_variables(),
                       expected_dfa->indices.data());
    actual_example =
        dfaMakeExample(actual_dfa->dfa_, 1, actual_dfa->get_nb_variables(),
                       actual_dfa->indices.data());
    // ensure either both null or both not null
    REQUIRE((expected_example == nullptr and actual_example == nullptr or
             expected_example != nullptr and actual_example != nullptr));
    if (expected_example) {
      auto expected_shortest_positive_example = std::string(actual_example);
      auto actual_shortest_positive_example = std::string(expected_example);
      REQUIRE(expected_shortest_positive_example ==
              actual_shortest_positive_example);
    }
    expected_example =
        dfaMakeExample(expected_dfa->dfa_, -1, expected_dfa->get_nb_variables(),
                       expected_dfa->indices.data());
    actual_example =
        dfaMakeExample(actual_dfa->dfa_, -1, actual_dfa->get_nb_variables(),
                       actual_dfa->indices.data());
    REQUIRE((expected_example == nullptr and actual_example == nullptr or
             expected_example != nullptr and actual_example != nullptr));
    if (expected_example) {
      auto expected_shortest_negative_example = std::string(actual_example);
      auto actual_shortest_negative_example = std::string(expected_example);
      REQUIRE(expected_shortest_negative_example ==
              actual_shortest_negative_example);
    }
  }
}

TEST_CASE("Test 'lisa_examples' dataset",
          "[integration][syft][lisa_examples]") {
  const auto dataset_path = DATASETS_ROOT / "lisa_examples";
  _dataset_test(dataset_path);
}
TEST_CASE("Test 'ltlf_1' dataset", "[integration][syft][ltlf_1]") {
  const auto dataset_path = DATASETS_ROOT / "ltlf_1";
  _dataset_test(dataset_path);
}

} // namespace whitemech::lydia