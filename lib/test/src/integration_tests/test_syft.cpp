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

#include "test/src/utils/to_dfa.hpp"
#include <catch.hpp>
#include <filesystem>
#include <lydia/dfa/mona_dfa.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>

namespace whitemech::lydia {

static Logger log("dataset-ltlf_1");
static const auto ROOT = std::filesystem::path("..") / ".." / "..";
static const auto DATASETS_ROOT =
    ROOT / "lib" / "test" / "src" / "assets" / "datasets";

std::shared_ptr<abstract_dfa> load_mona_dfa(std::filesystem::path filepath) {
  char **names;
  int *orders;
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

  // TODO remove state '0' and make '1' initial, only if states > 1

  return std::make_shared<mona_dfa>(dfa, names_vector);
}

static void _dataset_test(const std::filesystem::path &dataset_path) {
  log.info("Processing dataset " + dataset_path.string());
  const auto mona_dataset = dataset_path / "mona";
  const auto lydia_dataset = dataset_path / "lydia";

  std::filesystem::directory_iterator it(mona_dataset);
  for (const auto &datapath : it) {
    log.info("Processing file " + dataset_path.string());
    int var;
    int *indices;
    const auto strategy = std::make_shared<CompositionalStrategy>();
    const auto filename = datapath.path().filename().stem().string();
    const auto mona_file = mona_dataset / fmt::format("{}.mona", filename);
    const auto lydia_file = lydia_dataset / fmt::format("{}.ldlf", filename);
    DFA *at_least_one_step =
        dfaLDLfDiamondProp(dfaPropositionalTrue(), dfaLDLfTrue(), var, indices);

    // Compute MONA DFA
    auto expected_dfa =
        std::static_pointer_cast<mona_dfa>(load_mona_dfa(mona_file));
    //    auto temp_mona_df      = std::make_shared<mona_dfa>(expected_mona_dfa,
    //    temp_mona_dfa->names);

    // Compute Lydia Dfa
    // add "[true]end" in AND
    log.info("Lydia: Computing automaton " + lydia_file.string());
    auto temp_lydia_dfa = std::static_pointer_cast<mona_dfa>(
        to_dfa_from_formula_file(lydia_file, *strategy));
    var = temp_lydia_dfa->get_nb_variables();
    indices = temp_lydia_dfa->indices.data();
    auto actual_lydia_dfa =
        dfaProduct(temp_lydia_dfa->dfa_, at_least_one_step, dfaAND);
    actual_lydia_dfa = dfaMinimize(actual_lydia_dfa);
    auto actual_dfa =
        std::make_shared<mona_dfa>(actual_lydia_dfa, temp_lydia_dfa->names);

    int expected_nb_vars = expected_dfa->get_nb_variables();
    int actual_nb_vars = actual_dfa->get_nb_variables();
    REQUIRE(expected_nb_vars == actual_nb_vars);
    //  commented because of the additional 'zero' state in mona.
    int expected_nb_states = expected_dfa->get_nb_states();
    int actual_nb_states = actual_dfa->get_nb_states();
    REQUIRE((expected_nb_states == actual_nb_states + 1 or
             expected_nb_states == actual_nb_states));
    // REQUIRE(compare<1>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<2>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<3>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<4>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<5>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<6>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<7>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<8>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<9>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
    REQUIRE(compare<10>(*expected_dfa, *actual_dfa, actual_nb_vars, equal));
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