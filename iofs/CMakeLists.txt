cmake_minimum_required(VERSION 3.1)

project(iofs C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -std=c99")

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Libticables REQUIRED)
find_package(Libticalcs REQUIRED)
find_package(GLIB REQUIRED)
find_package(FUSE REQUIRED)

set(LIBS
    ${LIBS}
    ${LIBTICABLES_LIBRARIES}
    ${LIBTICALCS_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${FUSE_LIBRARIES}
)

add_executable(iofs src/main.c src/protocol.c src/operations.c)

target_include_directories(iofs PRIVATE
    src/
    ${LIBTICABLES_INCLUDES}
    ${LIBTICALCS_INCLUDES}
    ${GLIB_INCLUDES}
    ${FUSE_INCLUDES}
)

target_link_libraries(iofs ${LIBS})

ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_BINARY_DIR}/iofs.1
    COMMAND scdoc < ${CMAKE_SOURCE_DIR}/iofs.1.scdoc > ${CMAKE_BINARY_DIR}/iofs.1
  DEPENDS ${CMAKE_SOURCE_DIR}/iofs.1.scdoc
)

ADD_CUSTOM_TARGET(man ALL
    DEPENDS ${CMAKE_BINARY_DIR}/iofs.1)

INSTALL(
  FILES ${CMAKE_BINARY_DIR}/iofs.1
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1
)

INSTALL(
  TARGETS iofs
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
