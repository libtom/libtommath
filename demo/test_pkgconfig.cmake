# SPDX-License-Identifier: Unlicense

# Keep absolute install paths outside the source and build trees. These paths
# need not exist: the test configures packages but does not install anything.
get_filename_component(ROOT "${LTM_BINARY_DIR}" ABSOLUTE)
set(PARENT "")
while(NOT PARENT STREQUAL ROOT)
    set(PARENT "${ROOT}")
    get_filename_component(ROOT "${ROOT}" DIRECTORY)
endwhile()

foreach(LAYOUT relative absolute absolute-include absolute-lib)
    set(BUILD_DIR "${LTM_BINARY_DIR}/${LAYOUT}")
    set(INCLUDEDIR include)
    set(LIBDIR lib)
    set(EXPECTED_INCLUDEDIR "\${prefix}/include/libtommath")
    set(EXPECTED_LIBDIR "\${prefix}/lib")
    if(LAYOUT STREQUAL absolute OR LAYOUT STREQUAL absolute-include)
        set(INCLUDEDIR "${ROOT}libtommath-pkg-config-test/${LAYOUT}/headers")
        set(EXPECTED_INCLUDEDIR "${INCLUDEDIR}/libtommath")
    endif()
    if(LAYOUT STREQUAL absolute OR LAYOUT STREQUAL absolute-lib)
        set(LIBDIR "${ROOT}libtommath-pkg-config-test/${LAYOUT}/libraries")
        set(EXPECTED_LIBDIR "${LIBDIR}")
    endif()

    file(MAKE_DIRECTORY "${BUILD_DIR}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" "${LTM_SOURCE_DIR}"
            -G "${CMAKE_GENERATOR}"
            "-DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DCMAKE_GENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
            "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
            "-DCMAKE_INSTALL_PREFIX=${BUILD_DIR}/prefix"
            "-DCMAKE_INSTALL_INCLUDEDIR=${INCLUDEDIR}"
            "-DCMAKE_INSTALL_LIBDIR=${LIBDIR}"
            -DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON
            -DBUILD_SHARED_LIBS=ON
            -DBUILD_TESTING=OFF
        WORKING_DIRECTORY "${BUILD_DIR}"
        RESULT_VARIABLE RESULT
    )
    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR "Could not configure ${LAYOUT} installation")
    endif()

    file(READ "${BUILD_DIR}/libtommath.pc" CONFIG)
    foreach(DIR INCLUDEDIR LIBDIR)
        string(TOLOWER "${DIR}" VARIABLE)
        string(FIND "${CONFIG}" "${VARIABLE}=${EXPECTED_${DIR}}\n" MATCH)
        if(MATCH EQUAL -1)
            message(FATAL_ERROR "Incorrect ${VARIABLE} in ${LAYOUT} libtommath.pc")
        endif()
    endforeach()
endforeach()
