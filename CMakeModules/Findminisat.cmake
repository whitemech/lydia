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

# Try to find MiniSAT headers and libraries.
#
# Usage of this module as follows:
#
# find_package(minisat)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
# MINISAT_ROOT Set this variable to the root installation of
# libminisat if the module has problems finding the
# proper installation path.
#
# Variables defined by this module:
#
# MINISAT_FOUND System has MiniSAT libraries and headers
# MINISAT_LIBRARIES The MiniSAT library
# MINISAT_INCLUDE_DIRS The location of MiniSAT headers

# Get hint from environment variable (if any)
if(NOT MINISAT_ROOT AND DEFINED ENV{MINISAT_ROOT})
	set(MINISAT_ROOT "$ENV{MINISAT_ROOT}" CACHE PATH "MINISAT base directory location (optional, used for nonstandard installation paths)")
	mark_as_advanced(MINISAT_ROOT)
else()
	set(MINISAT_ROOT "/usr/local")
endif()

# Search path for nonstandard locations
if(MINISAT_ROOT)
	set(MINISAT_INCLUDE_PATH PATHS "${MINISAT_ROOT}/include" NO_DEFAULT_PATH)
	set(MINISAT_LIBRARY_PATH PATHS "${MINISAT_ROOT}/lib" NO_DEFAULT_PATH)
endif()

find_path(MINISAT_INCLUDE_DIRS NAMES minisat/core/Solver.h HINTS ${MINISAT_INCLUDE_PATH})
find_library(MINISAT_LIBRARY NAMES minisat)
set(MINISAT_LIBRARIES  ${MINISAT_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(minisat DEFAULT_MSG MINISAT_LIBRARIES MINISAT_INCLUDE_DIRS)

mark_as_advanced(MINISAT_ROOT MINISAT_LIBRARIES MINISAT_INCLUDE_DIRS)
