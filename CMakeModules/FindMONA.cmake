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

# Try to find MONA headers and libraries.
#
# Usage of this module as follows:
#
# find_package(MONA)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
# MONA_ROOT Set this variable to the root installation of
# libmona if the module has problems finding the
# proper installation path.
#
# Variables defined by this module:
#
# MONA_FOUND System has MONA libraries and headers
# MONA_LIBRARIES The MONA library
# MONA_INCLUDE_DIRS The location of MONA headers

# Get hint from environment variable (if any)
if(NOT MONA_ROOT AND DEFINED ENV{MONA_ROOT})
	set(MONA_ROOT "$ENV{MONA_ROOT}" CACHE PATH "MONA base directory location (optional, used for nonstandard installation paths)")
	mark_as_advanced(MONA_ROOT)
else()
	set(MONA_ROOT "/usr/local")
endif()

# Search path for nonstandard locations
if(MONA_ROOT)
	set(MONA_INCLUDE_PATH PATHS "${MONA_ROOT}/include" NO_DEFAULT_PATH)
	set(MONA_LIBRARY_PATH PATHS "${MONA_ROOT}/lib" NO_DEFAULT_PATH)
endif()

find_path(MONA_INCLUDE_DIRS NAMES mona/dfa.h HINTS ${MONA_INCLUDE_PATH})
find_library(MONA_MEM_LIBRARY NAMES monamem)
find_library(MONA_BDD_LIBRARY NAMES monabdd)
find_library(MONA_DFA_LIBRARY NAMES monadfa)
find_library(MONA_GTA_LIBRARY NAMES monagta)
set(MONA_LIBRARIES  ${MONA_MEM_LIBRARY}  ${MONA_BDD_LIBRARY} ${MONA_GTA_LIBRARY} ${MONA_DFA_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(MONA DEFAULT_MSG MONA_LIBRARIES MONA_INCLUDE_DIRS)

mark_as_advanced(MONA_ROOT MONA_LIBRARIES MONA_INCLUDE_DIRS)
