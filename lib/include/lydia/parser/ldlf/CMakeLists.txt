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

# generate lexer and parser
# LDLf parser
FLEX_TARGET(lydia_ldlf_lexer include/lydia/parser/ldlf/lexer.l ${CMAKE_CURRENT_SOURCE_DIR}/include/lydia/parser/ldlf/lexer.yy.cc)
BISON_TARGET(lydia_ldlf_parser include/lydia/parser/ldlf/parser.yy ${CMAKE_CURRENT_SOURCE_DIR}/include/lydia/parser/ldlf/parser.tab.cc )
ADD_FLEX_BISON_DEPENDENCY(lydia_ldlf_lexer lydia_ldlf_parser)
