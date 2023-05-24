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

# CMake build : global project

cmake_minimum_required(VERSION 3.10)

project (Lydia
         LANGUAGES CXX
         HOMEPAGE_URL https://github.com/whitemech/lydia.git
         DESCRIPTION "Lydia is a tool for LDLf translation to DFA and for LDLf synthesis.")
set(PROJECT_VERSION "0.1.3")
set(${PROJECT_NAME}_VERSION ${PROJECT_VERSION})
add_definitions( -DLYDIA_VERSION="${PROJECT_VERSION}" )
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # require at least gcc 4.8
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
        message(FATAL_ERROR "GCC version must be at least 8!")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # require at least clang 3.2
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6)
        message(FATAL_ERROR "Clang version must be at least 6!")
    endif()
else()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with Clang and GCC.")
endif()


set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message("-- Using build type ${CMAKE_BUILD_TYPE}")


# TODO introduce '-Werror', eventually
#set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")

# these cannot be added due to Flex/Bison
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -fno-exceptions")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -fno-rtti")

if ((ENABLE_COVERAGE) AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    message("-- Code coverage enabled")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ftest-coverage")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

if (PROFILE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
endif()

# TODO include this eventually
#set(CMAKE_CXX_CLANG_TIDY
#        clang-tidy;
#        -header-filter=.;
#        -checks=*;
#        -warnings-as-errors=*;)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_MODULE_PATH
        "${CMAKE_MODULE_PATH}"
        "${CMAKE_ROOT_DIR}/cmake/Modules"
        "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set (CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

# by default, Syft is enabled
if (NOT DEFINED WITH_SYFT)
    set(WITH_SYFT ON)
endif()
# by default, tests are enabled
if (NOT DEFINED LYDIA_ENABLE_TESTS)
    set(LYDIA_ENABLE_TESTS ON)
endif()

set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)
find_package(CUDD REQUIRED)
find_package(MONA REQUIRED)
find_package(Graphviz REQUIRED)
find_package(BISON 3.0 REQUIRED)
find_package(FLEX 2.6 REQUIRED)

if (WITH_SYFT)
    find_package(Syft REQUIRED)
endif()

add_subdirectory (third_party EXCLUDE_FROM_ALL)
add_subdirectory (lib)
add_subdirectory (app)
add_subdirectory (benchmark)

if (NOT LYDIA_ENABLE_TESTS)
    enable_testing ()
endif()

