# SPDX-License-Identifier: Unlicense
#
# LibTomMath, a free open source portable number theoretic multiple-precision
# integer (MPI) library written entirely in C.
#

cmake_minimum_required(VERSION 3.7)
project(tommath VERSION 1.2.0)

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
set(LTM_C_FLAGS  ${LTM_C_FLAGS} -Wstrict-prototypes -Wpointer-arith)

# Do we run the sanitizer?
if(DEFINED ENV{SANITIZER})
    set(LTM_C_FLAGS  ${LTM_C_FLAGS} -fsanitize=undefined -fno-sanitize-recover=all -fno-sanitize=float-divide-by-zero)
endif()

if(DEFINED ENV{CONV_WARNINGS})
    set(LTM_C_FLAGS  ${LTM_C_FLAGS} -std=c89 -Wconversion -Wsign-conversion)
    if($ENV{CONV_WARNINGS} EQUAL "strict")
        set(LTM_C_FLAGS  ${LTM_C_FLAGS} -Wc++-compat)
    endif()
else()
    set(LTM_C_FLAGS  ${LTM_C_FLAGS} -Wsystem-headers)
endif()

if(DEFINED ENV{COMPILE_DEBUG})
    set(LTM_C_FLAGS  ${LTM_C_FLAGS} -g3)
endif()

if(DEFINED ENV{COMPILE_SIZE})
    set(LTM_C_FLAGS  ${LTM_C_FLAGS} -Os)
else()
    if(NOT DEFINED ENV{IGNORE_SPEED})
        set(LTM_C_FLAGS  ${LTM_C_FLAGS} -O3 -funroll-loops)
        #x86 optimizations [should be valid for any GCC install though]
        set(LTM_C_FLAGS  ${LTM_C_FLAGS} -fomit-frame-pointer)
    endif()
    # TODO:
    # if(DEFINED ENV{COMPILE_LTO})
    #     set(LTM_C_FLAGS  "${LTM_C_FLAGS} -flto")
    #     set(LTM_LD_FLAGS "${LTM_LD_FLAGS} -flto")
    #     #AR = $(subst clang,llvm-ar,$(subst gcc,gcc-ar,$(CC)))
    # endif()
endif()

# TODO
# Are the coming three checks really the best way?

# What compiler do we have and what are their...uhm... peculiarities
# TODO: is the check for a C++ compiler necessary?
if( (CMAKE_C_COMPILER_ID MATCHES "(C|c?)lang") OR (CMAKE_CXX_COMPILER_ID MATCHES "(C|c?)lang"))
    set(LTM_C_FLAGS  ${LTM_C_FLAGS} -Wno-typedef-redefinition -Wno-tautological-compare -Wno-builtin-requires-header)
endif()

if( (CMAKE_C_COMPILER_ID MATCHES "mingw") OR (CMAKE_CXX_COMPILER_ID MATCHES "mingw"))
    set(LTM_C_FLAGS  ${LTM_C_FLAGS}  -Wno-shadow)
endif()

if(DEFINED ENV{PLATFORM})
    if($ENV{PLATFORM} MATCHES "Darwin")
        set(LTM_C_FLAGS  ${LTM_C_FLAGS}  -Wno-nullability-completeness)
    endif()
    if($ENV{PLATFORM} MATCHES "CYGWIN")
        set(LTM_C_FLAGS  ${LTM_C_FLAGS} -no-undefined)
    endif()
endif()

# TODO: coverage (lgcov)

# If the user set the environment variables at generate-time, append them
# in order to allow overriding our defaults.
if(DEFINED ENV{LTM_CFLAGS})
   set(LTM_C_FLAGS  ${LTM_C_FLAGS} $ENV{LTM_CFLAGS})
endif()
if(DEFINED ENV{LTM_LDFLAGS})
   set(LTM_LD_FLAGS  ${LTM_LD_FLAGS} $ENV{LTM_LDFLAGS})
endif()

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

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

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
# package release version
set(PACKAGE_RELEASE_VERSION 1)

# determine distribution and architecture
find_program(LSB_RELEASE lsb_release)

execute_process(COMMAND uname -m OUTPUT_VARIABLE MACHINE_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)

if(LSB_RELEASE)
    execute_process(COMMAND lsb_release -sr OUTPUT_VARIABLE LINUX_DISTRO_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND lsb_release -si OUTPUT_VARIABLE LINUX_DISTRO OUTPUT_STRIP_TRAILING_WHITESPACE)

    string(TOLOWER ${LINUX_DISTRO} LINUX_DISTRO)
endif()

# default CPack generators
set(CPACK_GENERATOR TGZ STGZ)

# extra CPack generators
if(LINUX_DISTRO STREQUAL "debian" OR LINUX_DISTRO STREQUAL "ubuntu" OR LINUX_DISTRO STREQUAL "linuxmint")
    list(APPEND CPACK_GENERATOR DEB)
elseif(LINUX_DISTRO STREQUAL "fedora" OR LINUX_DISTRO STREQUAL "opensuse" OR LINUX_DISTRO STREQUAL "centos")
    list(APPEND CPACK_GENERATOR RPM)
endif()

# general CPack config
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
message(STATUS "CPack: packages will be generated under ${CPACK_PACKAGE_DIRECTORY}")
set(CPACK_PACKAGE_NAME "lib${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LibTomMath")
set(CPACK_PACKAGE_VENDOR "LibTomMath")
set(CPACK_PACKAGE_CONTACT "libtom@googlegroups.com")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${PACKAGE_RELEASE_VERSION}_${LINUX_DISTRO}-${LINUX_DISTRO_VERSION}_${MACHINE_ARCH})
set(CPACK_STRIP_FILES ON)

# deb specific CPack config
set(CPACK_DEBIAN_PACKAGE_RELEASE ${PACKAGE_RELEASE_VERSION})
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://www.libtom.net/LibTomMath")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")

# rpm specific CPack config
set(CPACK_RPM_PACKAGE_URL "https://www.libtom.net/LibTomMath")
set(CPACK_RPM_PACKAGE_RELEASE ${PACKAGE_RELEASE_VERSION})
set(CPACK_RPM_PACKAGE_ARCHITECTURE ${MACHINE_ARCH})
set(CPACK_RPM_PACKAGE_NAME "lib${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_RPM_FILE_NAME "lib${PROJECT_NAME}_${PROJECT_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}_${LINUX_DISTRO}-${LINUX_DISTRO_VERSION}_${CPACK_RPM_PACKAGE_ARCHITECTURE}.rpm")
set(CPACK_RPM_PACKAGE_LICENSE "WTFPL")

include(CPack)
