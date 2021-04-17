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

# Try to find CUDD headers and libraries.
#
# Usage of this module as follows:
#
# find_package(CUDD)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
# CUDD_ROOT Set this variable to the root installation of
# libcudd if the module has problems finding the
# proper installation path.
#
# Variables defined by this module:
#
# CUDD_FOUND System has CUDD libraries and headers
# CUDD_LIBRARIES The CUDD library
# CUDD_INCLUDE_DIRS The location of CUDD headers

# Get hint from environment variable (if any)
if(NOT CUDD_ROOT AND DEFINED ENV{CUDD_ROOT})
	set(CUDD_ROOT "$ENV{CUDD_ROOT}" CACHE PATH "CUDD base directory location (optional, used for nonstandard installation paths)")
	mark_as_advanced(CUDD_ROOT)
else()
	set(CUDD_ROOT "/usr/local")
endif()

#set(CUDD_ROOT "$ENV{HOME}/install")
set(CUDD_ROOT "/usr/local")
# Search path for nonstandard locations
if(CUDD_ROOT)
	set(CUDD_INCLUDE_PATH PATHS "${CUDD_ROOT}/include" NO_DEFAULT_PATH)
	set(CUDD_LIBRARY_PATH PATHS "${CUDD_ROOT}/lib" NO_DEFAULT_PATH)
endif()

find_path(CUDD_INCLUDE_DIRS NAMES cudd.h cuddObj.hh HINTS ${CUDD_INCLUDE_PATH})
find_library(CUDD_LIBRARIES NAMES cudd CUDDVC-3.0.0 HINTS ${CUDD_LIBRARY_PATH})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(CUDD DEFAULT_MSG CUDD_LIBRARIES CUDD_INCLUDE_DIRS)

mark_as_advanced(CUDD_ROOT CUDD_LIBRARIES CUDD_INCLUDE_DIRS)
