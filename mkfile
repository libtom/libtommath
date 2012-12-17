</$objtype/mkfile
LIB=/$objtype/lib/ape/libtommath.a
OFILES=bn_error.$O bn_fast_mp_invmod.$O bn_fast_mp_montgomery_reduce.$O bn_fast_s_mp_mul_digs.$O \
	bn_fast_s_mp_mul_high_digs.$O bn_fast_s_mp_sqr.$O bn_mp_2expt.$O bn_mp_abs.$O \
	bn_mp_add.$O bn_mp_add_d.$O bn_mp_addmod.$O bn_mp_and.$O bn_mp_clamp.$O \
	bn_mp_clear.$O bn_mp_clear_multi.$O bn_mp_cmp.$O bn_mp_cmp_d.$O bn_mp_cmp_mag.$O \
	bn_mp_cnt_lsb.$O bn_mp_copy.$O bn_mp_count_bits.$O bn_mp_div.$O bn_mp_div_2.$O \
	bn_mp_div_2d.$O bn_mp_div_3.$O bn_mp_div_d.$O bn_mp_dr_is_modulus.$O bn_mp_dr_reduce.$O \
	bn_mp_dr_setup.$O bn_mp_exch.$O bn_mp_expt_d.$O bn_mp_exptmod.$O bn_mp_exptmod_fast.$O \
	bn_mp_exteuclid.$O bn_mp_fread.$O bn_mp_fwrite.$O bn_mp_gcd.$O bn_mp_get_int.$O \
	bn_mp_grow.$O bn_mp_init.$O bn_mp_init_copy.$O bn_mp_init_multi.$O bn_mp_init_set.$O \
	bn_mp_init_set_int.$O bn_mp_init_size.$O bn_mp_invmod.$O bn_mp_invmod_slow.$O \
	bn_mp_is_square.$O bn_mp_jacobi.$O bn_mp_karatsuba_mul.$O bn_mp_karatsuba_sqr.$O \
	bn_mp_lcm.$O bn_mp_lshd.$O bn_mp_mod.$O bn_mp_mod_2d.$O bn_mp_mod_d.$O \
	bn_mp_montgomery_calc_normalization.$O bn_mp_montgomery_reduce.$O bn_mp_montgomery_setup.$O \
	bn_mp_mul.$O bn_mp_mul_2.$O bn_mp_mul_2d.$O bn_mp_mul_d.$O bn_mp_mulmod.$O \
	bn_mp_n_root.$O bn_mp_neg.$O bn_mp_or.$O bn_mp_prime_fermat.$O bn_mp_prime_is_divisible.$O \
	bn_mp_prime_is_prime.$O bn_mp_prime_miller_rabin.$O bn_mp_prime_next_prime.$O \
	bn_mp_prime_rabin_miller_trials.$O bn_mp_prime_random_ex.$O bn_mp_radix_size.$O \
	bn_mp_radix_smap.$O bn_mp_rand.$O bn_mp_read_radix.$O bn_mp_read_signed_bin.$O \
	bn_mp_read_unsigned_bin.$O bn_mp_reduce.$O bn_mp_reduce_2k.$O bn_mp_reduce_2k_l.$O \
	bn_mp_reduce_2k_setup.$O bn_mp_reduce_2k_setup_l.$O bn_mp_reduce_is_2k.$O \
	bn_mp_reduce_is_2k_l.$O bn_mp_reduce_setup.$O bn_mp_rshd.$O bn_mp_set.$O \
	bn_mp_set_int.$O bn_mp_shrink.$O bn_mp_signed_bin_size.$O bn_mp_sqr.$O \
	bn_mp_sqrmod.$O bn_mp_sqrt.$O bn_mp_sub.$O bn_mp_sub_d.$O bn_mp_submod.$O \
	bn_mp_to_signed_bin.$O bn_mp_to_signed_bin_n.$O bn_mp_to_unsigned_bin.$O \
	bn_mp_to_unsigned_bin_n.$O bn_mp_toom_mul.$O bn_mp_toom_sqr.$O bn_mp_toradix.$O \
	bn_mp_toradix_n.$O bn_mp_unsigned_bin_size.$O bn_mp_xor.$O bn_mp_zero.$O \
	bn_prime_tab.$O bn_reverse.$O bn_s_mp_add.$O bn_s_mp_exptmod.$O bn_s_mp_mul_digs.$O \
	bn_s_mp_mul_high_digs.$O bn_s_mp_sqr.$O bn_s_mp_sub.$O bncore.$O 
HFILES=tommath.h tommath_class.h tommath_superclass.h tommath_class.h tommath_superclass.h \
	tommath_class.h tommath_superclass.h tommath_class.h /sys/include/ape/limits.h \
	/sys/include/ape/ctype.h /sys/include/ape/stddef.h /sys/include/ape/stdlib.h \
	/sys/include/ape/bsd.h /sys/include/ape/stddef.h /sys/include/ape/string.h \
	/sys/include/ape/sys/types.h /sys/include/ape/stddef.h /$objtype/include/ape/stdarg.h \
	/sys/include/ape/stdio.h 

</sys/src/cmd/mksyslib

CC=pcc
LD=pcc
CFLAGS=-c -I. -I/sys/include/ape -I/$objtype/include/ape -B -D_POSIX_SOURCE -D_SUSV2_SOURCE \
	-D_BSD_EXTENSION 


install:V:
	cp tommath.h /sys/include/ape/
	cp tommath_class.h /sys/include/ape/
	cp tommath_superclass.h /sys/include/ape/

nuke:V:
	mk clean
	rm -f $LIB
	rm -f /sys/include/ape/tommath.h
	rm -f /sys/include/ape/tommath_class.h
	rm -f /sys/include/ape/tommath_superclass.h

