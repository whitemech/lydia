#
# This file is part of Lydia.
#
# Lydia is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Lydia is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Lydia.  If not, see <https://www.gnu.org/licenses/>.
#

# CMake build : third party

#configure directories
set (THIRD_PARTY_MODULE_PATH "${PROJECT_SOURCE_DIR}/third_party")

# -------------------------------------------------------------------------
# catch

#configure directories
set (CATCH_MODULE_PATH "${THIRD_PARTY_MODULE_PATH}/Catch2")
set (CATCH_INCLUDE_PATH "${CATCH_MODULE_PATH}/single_include/catch2")

#include custom cmake function
include ( "${CATCH_MODULE_PATH}/contrib/ParseAndAddCatchTests.cmake")

# -------------------------------------------------------------------------
# spdlog

#configure directories
set (SPDLOG_MODULE_PATH "${THIRD_PARTY_MODULE_PATH}/spdlog")
set (SPDLOG_INCLUDE_PATH "${SPDLOG_MODULE_PATH}/include")

# -------------------------------------------------------------------------

# -------------------------------------------------------------------------
# fmt

#configure directories
set (FMT_MODULE_PATH "${THIRD_PARTY_MODULE_PATH}/fmt")
set (FMT_INCLUDE_PATH "${FMT_MODULE_PATH}/include")

# -------------------------------------------------------------------------

#set variables
set (THIRD_PARTY_INCLUDE_PATH  ${FMT_INCLUDE_PATH} ${SPDLOG_INCLUDE_PATH})

#set variables for tests
set (TEST_THIRD_PARTY_INCLUDE_PATH  ${CATCH_INCLUDE_PATH})

#export vars
set (THIRD_PARTY_INCLUDE_PATH  ${THIRD_PARTY_INCLUDE_PATH} PARENT_SCOPE)
set (TEST_THIRD_PARTY_INCLUDE_PATH  ${TEST_THIRD_PARTY_INCLUDE_PATH} PARENT_SCOPE)
