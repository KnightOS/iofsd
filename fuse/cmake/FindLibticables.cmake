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

find_path(LIBTICABLES_INCLUDES
  NAMES ticables.h tilp2/ticables.h)

find_library(LIBTICABLES_LIBRARIES NAMES ticables2 libticables2 ticables libticables)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBTICABLES DEFAULT_MSG LIBTICABLES_LIBRARIES LIBTICABLES_INCLUDES)
mark_as_advanced(LIBTICABLES_LIBRARIES LIBTICABLES_INCLUDES)
