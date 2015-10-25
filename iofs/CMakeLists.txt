cmake_minimum_required(VERSION 2.8.5)
project(iofs C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")
set(CMAKE_C_FLAGS "-g")
add_definitions("-Wall")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(Libticables REQUIRED)
find_package(GLIB REQUIRED)
find_package(FUSE REQUIRED)
set(LIBS ${LIBS} ${LIBTICABLES_LIBRARIES} ${GLIB_LIBRARIES} ${FUSE_LIBRARIES})

FILE(GLOB source ${PROJECT_SOURCE_DIR}/src/*.c)

add_executable(iofs
    ${source}
)

include_directories(
    src/
    ${LIBTICABLES_INCLUDES}
    ${GLIB_INCLUDES}
    ${FUSE_INCLUDES}
)

set(CMAKE_BUILD_TYPE Release)

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/iofs.1
  COMMAND a2x --no-xmllint --doctype manpage --format manpage ${CMAKE_CURRENT_SOURCE_DIR}/iofs.1.txt 
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/iofs.1.txt
)

ADD_CUSTOM_TARGET(man ALL
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/iofs.1)

INSTALL(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/iofs.1
  DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1
)

target_link_libraries(iofs ${LIBS})
