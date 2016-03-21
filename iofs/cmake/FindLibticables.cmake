# Note: this doesn't work and no one in the cmake community feels like helping

# - Find libticables
# Finds libticables includes and library
#
# LIBTICABLES_INCLUDES      - Headers
# LIBTICABLES_LIBRARIES     - Libraries
# LIBTICABLES_FOUND         - True if found

if(LIBTICABLES_INCLUDES)
  set (LIBTICABLES_FIND_QUIETLY TRUE)
endif()

find_path(LIBTICABLES_INCLUDES ticables.h PATH_SUFFIXES tilp2)

find_library(LIBTICABLES_LIBRARIES NAMES ticables2 libticables2 ticables libticables)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBTICABLES DEFAULT_MSG LIBTICABLES_LIBRARIES LIBTICABLES_INCLUDES)

if (LIBTICABLES_FOUND)
    message (STATUS "Found components for Libticables")
    message (STATUS "LIBTICABLES_INCLUDES = ${LIBTICABLES_INCLUDES}")
    message (STATUS "LIBTICABLES_LIBRARIES = ${LIBTICABLES_LIBRARIES}")
else (LIBTICABLES_FOUND)
    if (LIBTICABLES_FIND_REQUIRED)
        message (FATAL_ERROR "Could not find Libticables!")
    endif (LIBTICABLES_FIND_REQUIRED)
endif (LIBTICABLES_FOUND)

mark_as_advanced(LIBTICABLES_LIBRARIES LIBTICABLES_INCLUDES)
