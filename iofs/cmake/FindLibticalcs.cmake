# Note: this doesn't work and no one in the cmake community feels like helping

# - Find libticalcs
# Finds libticalcs includes and library
#
# LIBTICALCS_INCLUDES      - Headers
# LIBTICALCS_LIBRARIES     - Libraries
# LIBTICALCS_FOUND         - True if found

if(LIBTICALCS_INCLUDES)
  set (LIBTICALCS_FIND_QUIETLY TRUE)
endif()

find_path(LIBTICALCS_INCLUDES ticalcs.h PATH_SUFFIXES tilp2)

find_library(LIBTICALCS_LIBRARIES NAMES ticalcs2 libticalcs2 ticalcs libticalcs)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBTICALCS DEFAULT_MSG LIBTICALCS_LIBRARIES LIBTICALCS_INCLUDES)

if (LIBTICALCS_FOUND)
    message (STATUS "Found components for Libticalcs")
    message (STATUS "LIBTICALCS_INCLUDES = ${LIBTICALCS_INCLUDES}")
    message (STATUS "LIBTICALCS_LIBRARIES = ${LIBTICALCS_LIBRARIES}")
else (LIBTICALCS_FOUND)
    if (LIBTICALCS_FIND_REQUIRED)
        message (FATAL_ERROR "Could not find Libticalcs!")
    endif (LIBTICALCS_FIND_REQUIRED)
endif (LIBTICALCS_FOUND)

mark_as_advanced(LIBTICALCS_LIBRARIES LIBTICALCS_INCLUDES)
