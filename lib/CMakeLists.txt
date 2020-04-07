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

# generate
#BISON_TARGET(parser ./include/parser/parser.y ${CMAKE_CURRENT_BINARY_DIR}/include/parser/parser.c DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/parser/parser.h)
#FLEX_TARGET(lexer ./include/parser/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/include/parser/lexer.c DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/parser/lexer.h)
#ADD_FLEX_BISON_DEPENDENCY(lexer parser)

#configure directories
set (LIBRARY_MODULE_PATH "${PROJECT_SOURCE_DIR}/lib")
set (LIBRARY_SRC_PATH  "${LIBRARY_MODULE_PATH}/src" )
set (LIBRARY_INCLUDE_PATH  "${LIBRARY_MODULE_PATH}/include")

#set includes
include_directories (${LIBRARY_INCLUDE_PATH} ${THIRD_PARTY_INCLUDE_PATH})

#set sources
file (GLOB_RECURSE LIB_HEADER_FILES "${LIBRARY_INCLUDE_PATH}/*.hpp")
file (GLOB_RECURSE LIB_SOURCE_FILES "${LIBRARY_SRC_PATH}/*.cpp")
#set (LIB_PARSER_FILES ${BISON_parser_OUTPUTS} ${FLEX_lexer_OUTPUTS})

#set library
# TODO: add ${LIB_PARSER_FILES}
add_library (${LIB_NAME} STATIC ${LIB_SOURCE_FILES} ${LIB_HEADER_FILES})

#export vars
set (LIBRARY_INCLUDE_PATH  ${LIBRARY_INCLUDE_PATH} PARENT_SCOPE)
set (LIB_NAME ${LIB_NAME} PARENT_SCOPE)

#test
enable_testing ()
add_subdirectory (test)