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

# Get hint from environment variable (if any)
if(NOT CRYPTOMINISAT5_ROOT AND DEFINED ENV{CRYPTOMINISAT5_ROOT})
	set(CRYPTOMINISAT5_ROOT "$ENV{CRYPTOMINISAT5_ROOT}" CACHE PATH "CRYPTOMINISAT5 base directory location (optional, used for nonstandard installation paths)")
	mark_as_advanced(CRYPTOMINISAT5_ROOT)
endif()

#set(CUDD_ROOT "$ENV{HOME}/install")
set(CRYPTOMINISAT5_ROOT "/usr/local")
# Search path for nonstandard locations
if(CRYPTOMINISAT5_ROOT)
	set(CRYPTOMINISAT5_INCLUDE_PATH PATHS "${CRYPTOMINISAT5_ROOT}/include" NO_DEFAULT_PATH)
	set(CRYPTOMINISAT5_LIBRARY_PATH PATHS "${CRYPTOMINISAT5_ROOT}/lib" NO_DEFAULT_PATH)
endif()

find_path(CRYPTOMINISAT5_INCLUDE_DIR NAMES cryptominisat5/cryptominisat.h HINTS ${CRYPTOMINISAT5_INCLUDE_PATH})
find_library(CRYPTOMINISAT5_LIBRARY NAMES cryptominisat5 HINTS ${CRYPTOMINISAT5_LIBRARY_PATH})

set(CRYPTOMINISAT5_INCLUDE_DIRS ${CRYPTOMINISAT5_INCLUDE_DIR} ${CRYPTOMINISAT5_INCLUDE_DIR}/cryptominisat5)
set(CRYPTOMINISAT5_LIBRARIES ${CRYPTOMINISAT5_LIBRARY})


