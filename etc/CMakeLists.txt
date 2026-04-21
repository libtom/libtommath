# SPDX-License-Identifier: Unlicense
#
# LibTomMath, a free open source portable number theoretic multiple-precision
# integer (MPI) library written entirely in C.
#

cmake_minimum_required(VERSION 3.10)

set(LTM_TUNE tune-ltm)

# This file can be included from the top level or used stand-alone
if(PROJECT_NAME)
    set(LIBRARY_NAME ${PROJECT_NAME})
else()
    # Define an independent project and all the necessary stuff around
    project(${LTM_TUNE}
        LANGUAGES C)
    set(LIBRARY_NAME libtommath)
    find_package(${LIBRARY_NAME})
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE "Release")
    endif()
endif()

add_executable(tune
    ${CMAKE_CURRENT_SOURCE_DIR}/tune.c
)

target_include_directories(tune PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(tune PRIVATE
    ${LIBRARY_NAME}
)

target_compile_options(tune PRIVATE
    ${LTM_C_FLAGS}
)
target_link_options(tune BEFORE PUBLIC
    ${LTM_LD_FLAGS}
)

if(BUILD_GRAPHS)
   # used in tune_it.sh
   find_program(GNUPLOT gnuplot)
   add_custom_command(TARGET tune POST_BUILD COMMAND  ${CMAKE_CURRENT_SOURCE_DIR}/tune_it.sh 1000 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} VERBATIM)
else()
   add_custom_command(TARGET tune POST_BUILD COMMAND  ${CMAKE_CURRENT_SOURCE_DIR}/tune_it.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} VERBATIM)
endif()
