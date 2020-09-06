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

# CMake build : library

#configure variables
set (LIB_NAME "lydia")

# generate lexer and parser
BISON_TARGET(lydia_parser include/lydia/parser/parser.yy ${CMAKE_CURRENT_SOURCE_DIR}/include/lydia/parser/parser.tab.cc )
FLEX_TARGET(lydia_lexer include/lydia/parser/lexer.l ${CMAKE_CURRENT_SOURCE_DIR}/include/lydia/parser/lexer.yy.cc)
ADD_FLEX_BISON_DEPENDENCY(lydia_lexer lydia_parser)

#configure directories
set (LIBRARY_MODULE_PATH "${PROJECT_SOURCE_DIR}/lib")
set (LIBRARY_SRC_PATH  "${LIBRARY_MODULE_PATH}/src" )
set (LIBRARY_INCLUDE_PATH  "${LIBRARY_MODULE_PATH}/include")

#set includes
include_directories (${LIBRARY_INCLUDE_PATH} ${THIRD_PARTY_INCLUDE_PATH})
include_directories (${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

#set sources
file (GLOB_RECURSE LIB_HEADER_FILES "${LIBRARY_INCLUDE_PATH}/*.hpp" ${LIBRARY_INCLUDE_PATH}/*.h ${LIBRARY_INCLUDE_PATH}/*.hh)
file (GLOB_RECURSE LIB_SOURCE_FILES "${LIBRARY_SRC_PATH}/*.cpp" ${LIBRARY_INCLUDE_PATH}/*.cpp ${LIBRARY_INCLUDE_PATH}/*.cc)

#set library
add_library (${LIB_NAME}
        STATIC
        ${LIB_SOURCE_FILES}
        ${LIB_HEADER_FILES}
        ${BISON_lydia_parser_OUTPUTS}
        ${FLEX_lydia_lexer_OUTPUTS})

target_link_libraries(${LIB_NAME}
        ${CUDD_LIBRARIES}
        ${MONA_LIBRARIES}
        ${GRAPHVIZ_LIBRARIES}
        ${MINISAT_LIBRARIES}
        ${FLEX_LIBRARIES}
        ${BISON_LIBRARIES})

#export vars
set (LIBRARY_INCLUDE_PATH  ${LIBRARY_INCLUDE_PATH} PARENT_SCOPE)
set (LIB_NAME ${LIB_NAME} PARENT_SCOPE)

#test
enable_testing ()
add_subdirectory (test)
