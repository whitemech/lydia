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

# CMake build : library tests

#configure variables
set (TEST_APP_NAME "${LIB_NAME}Test")

#configure directories
set (TEST_MODULE_PATH "${LIBRARY_MODULE_PATH}/test")

#configure test directories
set (TEST_SRC_PATH  "${TEST_MODULE_PATH}/src" )

#set includes
include_directories (${LIBRARY_INCLUDE_PATH} ${TEST_THIRD_PARTY_INCLUDE_PATH})

#set test sources
file (GLOB_RECURSE TEST_SOURCE_FILES "${TEST_SRC_PATH}/*.cpp")
file (GLOB_RECURSE TEST_HEADER_FILES "${TEST_SRC_PATH}/*.hpp")

#set target executable
add_executable (${TEST_APP_NAME} ${TEST_SOURCE_FILES} ${TEST_HEADER_FILES})

#add the library
target_link_libraries (${TEST_APP_NAME}
        ${LIB_NAME}
        Threads::Threads)

# Turn on CMake testing capabilities
enable_testing()

#parse catch tests
ParseAndAddCatchTests (${TEST_APP_NAME})
