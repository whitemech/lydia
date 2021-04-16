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
set (BENCHMARK_APP_NAME "${LIB_NAME}Bench")

#configure directories
set (LIBRARY_MODULE_PATH "${PROJECT_SOURCE_DIR}/lib")
set (LIBRARY_SRC_PATH  "${LIBRARY_MODULE_PATH}/src" )
set (LIBRARY_INCLUDE_PATH  "${LIBRARY_MODULE_PATH}/include")
set (BENCHMARK_MODULE_PATH "${PROJECT_SOURCE_DIR}/benchmark")

#configure benchmark directories
set (BENCHMARK_SRC_PATH  "${BENCHMARK_MODULE_PATH}/src" )

#set includes
include_directories (${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${BENCHMARK_SRC_PATH} ${LIBRARY_INCLUDE_PATH} ${THIRD_PARTY_INCLUDE_PATH} ${TEST_THIRD_PARTY_INCLUDE_PATH})

#set test sources
file (GLOB BENCHMARK_SOURCE_FILES "${BENCHMARK_SRC_PATH}/*.cpp")

#set target executable
add_executable (${BENCHMARK_APP_NAME} ${BENCHMARK_SOURCE_FILES})

#add the library
target_link_libraries (${BENCHMARK_APP_NAME}
        ${LIB_NAME}
        ${CUDD_LIBRARIES}
        ${MONA_LIBRARIES}
        benchmark::benchmark
        Threads::Threads)

add_custom_target(lydia-benchmark
        COMMAND ${CMAKE_BINARY_DIR}/bin/${BENCHMARK_APP_NAME}
        DEPENDS lydia ${BENCHMARK_APP_NAME}
        COMMENT "Run benchmarks." VERBATIM)
