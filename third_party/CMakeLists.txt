#
# This file is part of Lydia.
#
# Lydia is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Lydia is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Lydia.  If not, see <https://www.gnu.org/licenses/>.
#

# CMake build : third party

#configure directories
set (THIRD_PARTY_MODULE_PATH "${PROJECT_SOURCE_DIR}/third_party")

# -------------------------------------------------------------------------
# catch
set (CATCH_MODULE_PATH "${THIRD_PARTY_MODULE_PATH}/Catch2")
set (CATCH_INCLUDE_PATH "${CATCH_MODULE_PATH}/single_include/catch2")
include ( "${CATCH_MODULE_PATH}/contrib/ParseAndAddCatchTests.cmake")
# -------------------------------------------------------------------------
# google/benchmark
set (GOOGLE_BENCHMARK_MODULE_PATH "${THIRD_PARTY_MODULE_PATH}/google/benchmark")
set (GOOGLE_BENCHMARK_INCLUDE_PATH "${GOOGLE_BENCHMARK_MODULE_PATH}/include")
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
add_subdirectory(${GOOGLE_BENCHMARK_MODULE_PATH})
# -------------------------------------------------------------------------
# spdlog
set (SPDLOG_MODULE_PATH "${THIRD_PARTY_MODULE_PATH}/spdlog")
set (SPDLOG_INCLUDE_PATH "${SPDLOG_MODULE_PATH}/include")
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
# CLI11
set (CLI11_MODULE_PATH "${THIRD_PARTY_MODULE_PATH}/CLI11")
set (CLI11_INCLUDE_PATH "${CLI11_MODULE_PATH}/include")
# -------------------------------------------------------------------------
# cppitertools
set (CPPITERTOOLS_MODULE_PATH "${THIRD_PARTY_MODULE_PATH}/cppitertools/cppitertools")
set (CPPITERTOOLS_INCLUDE_PATH "${THIRD_PARTY_MODULE_PATH}/cppitertools")
# -------------------------------------------------------------------------
# CUDD
set (CUDD_INCLUDE_PATH ${CUDD_INCLUDE_DIRS})
# -------------------------------------------------------------------------
# MONA
set (MONA_INCLUDE_PATH ${MONA_INCLUDE_DIRS})
# -------------------------------------------------------------------------
# Syft
set (SYFT_INCLUDE_PATH ${SYFT_INCLUDE_DIRS})
# -------------------------------------------------------------------------

#set variables
set (THIRD_PARTY_INCLUDE_PATH
        ${SPDLOG_INCLUDE_PATH}
        ${CLI11_INCLUDE_PATH}
        ${CUDD_INCLUDE_PATH}
        ${MONA_INCLUDE_PATH}
        ${SYFT_INCLUDE_PATH}
        ${GOOGLE_BENCHMARK_INCLUDE_PATH}
        ${FLEX_INCLUDE_DIRS}
        ${BISON_INCLUDE_DIRS})

#set variables for tests
set (TEST_THIRD_PARTY_INCLUDE_PATH
        ${CATCH_INCLUDE_PATH}
        ${CPPITERTOOLS_INCLUDE_PATH})

#export vars
set (THIRD_PARTY_INCLUDE_PATH  ${THIRD_PARTY_INCLUDE_PATH} PARENT_SCOPE)
set (TEST_THIRD_PARTY_INCLUDE_PATH  ${TEST_THIRD_PARTY_INCLUDE_PATH} PARENT_SCOPE)
