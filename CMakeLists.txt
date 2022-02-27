# SPDX-License-Identifier: Unlicense
#
# LibTomMath, a free open source portable number theoretic multiple-precision
# integer (MPI) library written entirely in C.
#

cmake_minimum_required(VERSION 3.10)

project(tommath
    VERSION 1.2.0
    DESCRIPTION "A free open source portable number theoretic multiple-precision integer (MPI) library written entirely in C."
    HOMEPAGE_URL "https://www.libtom.net/LibTomMath"
    LANGUAGES C)

# package release version
# bump if re-releasing the same VERSION + patches
# set to 1 if releasing a new VERSION
set(PACKAGE_RELEASE_VERSION 1)

#-----------------------------------------------------------------------------
# Include cmake modules
#-----------------------------------------------------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(sources.cmake)

# The only direct cmake argument for now
option(BUILD_SHARED_LIBS "Build shared library and only the shared library if \"ON\", default is static" OFF)

#-----------------------------------------------------------------------------
# Compose CFLAGS
#-----------------------------------------------------------------------------

# Some information copied from makefile_include.mk

# Basic set
set(LTM_C_FLAGS  -Wall -Wsign-compare -Wextra -Wshadow)
set(LTM_C_FLAGS  ${LTM_C_FLAGS} -Wdeclaration-after-statement -Wbad-function-cast -Wcast-align)
set(LTM_C_FLAGS  ${LTM_C_FLAGS} -Wstrict-prototypes -Wpointer-arith -Wsystem-headers)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_C_FLAGS_DEBUG "-g3")
set(CMAKE_C_FLAGS_RELEASE "-O3 -funroll-loops -fomit-frame-pointer")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g3 -O2")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os")

if(COMPILE_LTO)
    set(LTM_C_FLAGS  ${LTM_C_FLAGS} -flto)
    set(LTM_LD_FLAGS ${LTM_LD_FLAGS} -flto)
endif()

# What compiler do we have and what are their...uhm... peculiarities
if(CMAKE_C_COMPILER_ID MATCHES "(C|c?)lang")
    set(LTM_C_FLAGS  ${LTM_C_FLAGS} -Wno-typedef-redefinition -Wno-tautological-compare -Wno-builtin-requires-header)
    # Clang requires at least '-O1' for dead code eliminiation
    set(CMAKE_C_FLAGS_DEBUG  "${CMAKE_C_FLAGS_DEBUG} -O1")
endif()
if(CMAKE_C_COMPILER MATCHES "mingw")
    set(LTM_C_FLAGS  ${LTM_C_FLAGS}  -Wno-shadow)
endif()
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(LTM_C_FLAGS  ${LTM_C_FLAGS}  -Wno-nullability-completeness)
endif()
if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN")
    set(LTM_C_FLAGS  ${LTM_C_FLAGS} -no-undefined)
endif()

# TODO: coverage (lgcov)

# If the user set the environment variables at generate-time, append them
# in order to allow overriding our defaults.
# ${LTM_CFLAGS} means the user passed it via sth like:
# $ cmake -DLTM_CFLAGS="foo"
set(LTM_C_FLAGS  ${LTM_C_FLAGS} ${LTM_CFLAGS})
set(LTM_LD_FLAGS  ${LTM_LD_FLAGS} ${LTM_LDFLAGS})

#-----------------------------------------------------------------------------
# library target
#-----------------------------------------------------------------------------
add_library(${PROJECT_NAME}
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)

target_compile_options(${PROJECT_NAME} BEFORE PRIVATE
    ${LTM_C_FLAGS}
)
target_link_options(${PROJECT_NAME} BEFORE PRIVATE
    ${LTM_LD_FLAGS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

#-----------------------------------------------------------------------------
# demo target
#-----------------------------------------------------------------------------


add_executable(test-target EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/shared.c
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/test.c
)

target_include_directories(test-target PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test-target PRIVATE
    ${PROJECT_NAME}
)

target_compile_options(test-target BEFORE PRIVATE
    $<$<STREQUAL:$<TARGET_PROPERTY:${PROJECT_NAME},TYPE>,SHARED_LIBRARY>:-O1 -DLTM_TEST_DYNAMIC>
    ${LTM_C_FLAGS}
)
target_link_options(test-target BEFORE PRIVATE
    $<$<STREQUAL:$<TARGET_PROPERTY:${PROJECT_NAME},TYPE>,SHARED_LIBRARY>:-O1>
    ${LTM_LD_FLAGS}
)

#-----------------------------------------------------------------------------
# demo target
#-----------------------------------------------------------------------------
add_custom_target(check
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test-target
)

add_dependencies(check test-target)

#-----------------------------------------------------------------------------
# Install/export targets and files
#-----------------------------------------------------------------------------
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(PROJECT_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake")
set(PROJECT_CONFIG_FILE "${PROJECT_NAME}-config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")

install(TARGETS ${PROJECT_NAME}
    EXPORT ${TARGETS_EXPORT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

# Install libtommath.pc for pkg-config if we build a shared library
if(BUILD_SHARED_LIBS)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/lib${PROJECT_NAME}.pc.in
        ${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.pc
        @ONLY
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig
    )
endif()

# generate package version file
write_basic_package_version_file(
    ${PROJECT_VERSION_FILE}
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# install version file
install(FILES ${PROJECT_VERSION_FILE}
        DESTINATION ${CONFIG_INSTALL_DIR}
)

# build directory package config
export(EXPORT ${TARGETS_EXPORT_NAME}
       FILE ${PROJECT_CONFIG_FILE}
)

# installed package config
install(EXPORT ${TARGETS_EXPORT_NAME}
        DESTINATION ${CONFIG_INSTALL_DIR}
        FILE ${PROJECT_CONFIG_FILE}
)

# add to CMake registry
export(PACKAGE ${PROJECT_NAME})

#---------------------------------------------------------------------------------------
# Create release packages
#---------------------------------------------------------------------------------------

# determine distribution and architecture
find_program(LSB_RELEASE lsb_release)

execute_process(COMMAND uname -m OUTPUT_VARIABLE MACHINE_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)

if(LSB_RELEASE)
    execute_process(COMMAND lsb_release -si OUTPUT_VARIABLE LINUX_DISTRO OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND lsb_release -sc OUTPUT_VARIABLE LINUX_DISTRO_CODENAME OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND lsb_release -sr OUTPUT_VARIABLE LINUX_DISTRO_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

    string(TOLOWER ${LINUX_DISTRO} LINUX_DISTRO)
    if(LINUX_DISTRO_CODENAME STREQUAL "n/a")
        set(DISTRO_PACK_PATH ${LINUX_DISTRO}/${LINUX_DISTRO_VERSION}/)
    else()
        set(DISTRO_PACK_PATH ${LINUX_DISTRO}/${LINUX_DISTRO_CODENAME}/)
    endif()
else()
    set(DISTRO_PACK_PATH ${CMAKE_SYSTEM_NAME}/)
endif()

# default CPack generators
set(CPACK_GENERATOR TGZ STGZ)

# extra CPack generators
if(LINUX_DISTRO STREQUAL "debian" OR LINUX_DISTRO STREQUAL "ubuntu" OR LINUX_DISTRO STREQUAL "linuxmint")
    list(APPEND CPACK_GENERATOR DEB)
elseif(LINUX_DISTRO STREQUAL "fedora" OR LINUX_DISTRO STREQUAL "opensuse" OR LINUX_DISTRO STREQUAL "centos")
    list(APPEND CPACK_GENERATOR RPM)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    list(APPEND CPACK_GENERATOR FREEBSD)
endif()

# general CPack config
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages/${DISTRO_PACK_PATH})
message(STATUS "CPack: packages will be generated under ${CPACK_PACKAGE_DIRECTORY}")
set(CPACK_PACKAGE_NAME "lib${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LibTomMath")
set(CPACK_PACKAGE_VENDOR "libtom projects")
set(CPACK_PACKAGE_CONTACT "libtom@googlegroups.com")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(PACKAGE_NAME_TRAILER ${CPACK_PACKAGE_VERSION}-${PACKAGE_RELEASE_VERSION}_${MACHINE_ARCH})
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${PACKAGE_NAME_TRAILER})
set(CPACK_STRIP_FILES ON)

# deb specific CPack config
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_RELEASE ${PACKAGE_RELEASE_VERSION})
if(BUILD_SHARED_LIBS)
    set(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}1")
    set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
else()
    set(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-dev")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
endif()

# rpm specific CPack config
set(CPACK_RPM_PACKAGE_RELEASE ${PACKAGE_RELEASE_VERSION})
set(CPACK_RPM_PACKAGE_ARCHITECTURE ${MACHINE_ARCH})
set(CPACK_RPM_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}")
set(CPACK_RPM_PACKAGE_LICENSE "The Unlicense")

# FreeBSD specific CPack config
set(CPACK_FREEBSD_PACKAGE_MAINTAINER "gahr@FreeBSD.org")
set(CPACK_FREEBSD_PACKAGE_ORIGIN "math/libtommath")
set(CPACK_FREEBSD_PACKAGE_CATEGORIES "math")

include(CPack)
