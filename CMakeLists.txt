CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(tommath VERSION 1.0)

set(TOMMATH_HEADERS
    tommath.h
    tommath_class.h
    tommath_private.h
    tommath_superclass.h
)

set(TOMMATH_SRCS
    bncore.c
    bn_error.c
    bn_fast_mp_invmod.c
    bn_fast_mp_montgomery_reduce.c
    bn_fast_s_mp_mul_digs.c
    bn_fast_s_mp_mul_high_digs.c
    bn_fast_s_mp_sqr.c
    bn_mp_2expt.c
    bn_mp_abs.c
    bn_mp_add.c
    bn_mp_add_d.c
    bn_mp_addmod.c
    bn_mp_and.c
    bn_mp_clamp.c
    bn_mp_clear.c
    bn_mp_clear_multi.c
    bn_mp_cmp.c
    bn_mp_cmp_d.c
    bn_mp_cmp_mag.c
    bn_mp_cnt_lsb.c
    bn_mp_copy.c
    bn_mp_count_bits.c
    bn_mp_div_2.c
    bn_mp_div_2d.c
    bn_mp_div_3.c
    bn_mp_div.c
    bn_mp_div_d.c
    bn_mp_dr_is_modulus.c
    bn_mp_dr_reduce.c
    bn_mp_dr_setup.c
    bn_mp_exch.c
    bn_mp_export.c
    bn_mp_expt_d.c
    bn_mp_expt_d_ex.c
    bn_mp_exptmod.c
    bn_mp_exptmod_fast.c
    bn_mp_exteuclid.c
    bn_mp_fread.c
    bn_mp_fwrite.c
    bn_mp_gcd.c
    bn_mp_get_int.c
    bn_mp_get_long.c
    bn_mp_get_long_long.c
    bn_mp_grow.c
    bn_mp_import.c
    bn_mp_init.c
    bn_mp_init_copy.c
    bn_mp_init_multi.c
    bn_mp_init_set.c
    bn_mp_init_set_int.c
    bn_mp_init_size.c
    bn_mp_invmod.c
    bn_mp_invmod_slow.c
    bn_mp_is_square.c
    bn_mp_jacobi.c
    bn_mp_karatsuba_mul.c
    bn_mp_karatsuba_sqr.c
    bn_mp_lcm.c
    bn_mp_lshd.c
    bn_mp_mod_2d.c
    bn_mp_mod.c
    bn_mp_mod_d.c
    bn_mp_montgomery_calc_normalization.c
    bn_mp_montgomery_reduce.c
    bn_mp_montgomery_setup.c
    bn_mp_mul_2.c
    bn_mp_mul_2d.c
    bn_mp_mul.c
    bn_mp_mul_d.c
    bn_mp_mulmod.c
    bn_mp_neg.c
    bn_mp_n_root.c
    bn_mp_n_root_ex.c
    bn_mp_or.c
    bn_mp_prime_fermat.c
    bn_mp_prime_is_divisible.c
    bn_mp_prime_is_prime.c
    bn_mp_prime_miller_rabin.c
    bn_mp_prime_next_prime.c
    bn_mp_prime_rabin_miller_trials.c
    bn_mp_prime_random_ex.c
    bn_mp_radix_size.c
    bn_mp_radix_smap.c
    bn_mp_rand.c
    bn_mp_read_radix.c
    bn_mp_read_signed_bin.c
    bn_mp_read_unsigned_bin.c
    bn_mp_reduce_2k.c
    bn_mp_reduce_2k_l.c
    bn_mp_reduce_2k_setup.c
    bn_mp_reduce_2k_setup_l.c
    bn_mp_reduce.c
    bn_mp_reduce_is_2k.c
    bn_mp_reduce_is_2k_l.c
    bn_mp_reduce_setup.c
    bn_mp_rshd.c
    bn_mp_set.c
    bn_mp_set_int.c
    bn_mp_set_long.c
    bn_mp_set_long_long.c
    bn_mp_shrink.c
    bn_mp_signed_bin_size.c
    bn_mp_sqr.c
    bn_mp_sqrmod.c
    bn_mp_sqrt.c
    bn_mp_sqrtmod_prime.c
    bn_mp_sub.c
    bn_mp_sub_d.c
    bn_mp_submod.c
    bn_mp_toom_mul.c
    bn_mp_toom_sqr.c
    bn_mp_toradix.c
    bn_mp_toradix_n.c
    bn_mp_to_signed_bin.c
    bn_mp_to_signed_bin_n.c
    bn_mp_to_unsigned_bin.c
    bn_mp_to_unsigned_bin_n.c
    bn_mp_unsigned_bin_size.c
    bn_mp_xor.c
    bn_mp_zero.c
    bn_prime_tab.c
    bn_reverse.c
    bn_s_mp_add.c
    bn_s_mp_exptmod.c
    bn_s_mp_mul_digs.c
    bn_s_mp_mul_high_digs.c
    bn_s_mp_sqr.c
    bn_s_mp_sub.c
)

set(TOMMATH_ALL_SRCS
    ${TOMMATH_SRCS}
    ${TOMMATH_HEADERS}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


set(BUILD_DYNAMIC OFF CACHE BOOL "build dynamic lib instead of static one")


if(BUILD_DYNAMIC)
    set(TOMMATH_BUILD_TYPE SHARED)

    if(WIN32)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
else()
    set(TOMMATH_BUILD_TYPE STATIC)
endif()


add_library(tommath ${TOMMATH_BUILD_TYPE} ${TOMMATH_ALL_SRCS})


#######################################
#       cmake config-mode stuff       #
#######################################

set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated/cmake")

set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)

# Note: PROJECT_VERSION is used as a VERSION
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Note: variable 'targets_export_name' used
configure_file("Config.cmake.in" "${project_config}" @ONLY)


install(
    TARGETS tommath
    EXPORT "${targets_export_name}"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "${include_install_dir}"
)

install(
    FILES ${TOMMATH_HEADERS}
    DESTINATION "${include_install_dir}"
)


install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)


install(
    EXPORT "${targets_export_name}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)
