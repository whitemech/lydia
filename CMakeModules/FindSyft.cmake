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

# Try to find SYFT headers and libraries.
#
# Usage of this module as follows:
#
# find_package(SYFT)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
# SYFT_ROOT Set this variable to the root installation of
# libmona if the module has problems finding the
# proper installation path.
#
# Variables defined by this module:
#
# SYFT_FOUND System has SYFT libraries and headers
# SYFT_LIBRARIES The SYFT library
# SYFT_INCLUDE_DIRS The location of SYFT headers

# Get hint from environment variable (if any)
if(NOT SYFT_ROOT AND DEFINED ENV{SYFT_ROOT})
	set(SYFT_ROOT "$ENV{SYFT_ROOT}" CACHE PATH "SYFT base directory location (optional, used for nonstandard installation paths)")
	mark_as_advanced(SYFT_ROOT)
else()
	set(SYFT_ROOT "/usr/local")
endif()

# Search path for nonstandard locations
if(SYFT_ROOT)
	set(SYFT_INCLUDE_PATH PATHS "${SYFT_ROOT}/include" NO_DEFAULT_PATH)
	set(SYFT_LIBRARY_PATH PATHS "${SYFT_ROOT}/lib" NO_DEFAULT_PATH)
endif()

find_path(SYFT_INCLUDE_DIRS NAMES synthesis/DFA.h HINTS ${SYFT_INCLUDE_PATH})
find_library(SYFT_SYNTHESIS_LIBRARY NAMES synthesis)
set(SYFT_LIBRARIES  ${SYFT_SYNTHESIS_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Syft DEFAULT_MSG SYFT_LIBRARIES SYFT_INCLUDE_DIRS)

mark_as_advanced(SYFT_ROOT SYFT_LIBRARIES SYFT_INCLUDE_DIRS)
