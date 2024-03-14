# SPDX-License-Identifier: Unlicense
#
# LibTomMath, a free open source portable number theoretic multiple-precision
# integer (MPI) library written entirely in C.
#

cmake_minimum_required(VERSION 3.10)

set(LTM_TEST test-ltm)

# This file can be included from the top level or used stand-alone
if(PROJECT_NAME)
    set(LIBRARY_NAME ${PROJECT_NAME})
else()
    # Define an independent project and all the necessary stuff around
    project(${LTM_TEST}
        LANGUAGES C)
    set(LIBRARY_NAME libtommath)
    find_package(${LIBRARY_NAME})
    include(CTest)
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE "Release")
    endif()
endif()

#-----------------------------------------------------------------------------
# Compose CFLAGS etc.
#-----------------------------------------------------------------------------

if(NOT MSVC)
    set(CMAKE_C_FLAGS_DEBUG "-g3 -O1")
endif()

#-----------------------------------------------------------------------------
# demo target
#-----------------------------------------------------------------------------

add_executable(${LTM_TEST}
    ${CMAKE_CURRENT_SOURCE_DIR}/shared.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test.c
)

target_include_directories(${LTM_TEST} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<$<STREQUAL:${PROJECT_NAME},${LTM_TEST}>:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(${LTM_TEST} PRIVATE
    ${LIBRARY_NAME}
)

target_compile_options(${LTM_TEST} PRIVATE
    $<$<STREQUAL:$<TARGET_PROPERTY:${LIBRARY_NAME},TYPE>,SHARED_LIBRARY>:-DLTM_TEST_DYNAMIC>
    ${LTM_C_FLAGS}
)
target_link_options(${LTM_TEST} BEFORE PUBLIC
    ${LTM_LD_FLAGS}
)

#-----------------------------------------------------------------------------
# CTest
#-----------------------------------------------------------------------------
add_test(NAME ${LTM_TEST} COMMAND ${LTM_TEST})

find_program(MEMORYCHECK_COMMAND valgrind)
set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full")
