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
