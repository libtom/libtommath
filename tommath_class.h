#if !(defined(LTM1) && defined(LTM2) && defined(LTM3))
#if defined(LTM2)
#define LTM3
#endif
#if defined(LTM1)
#define LTM2
#endif
#define LTM1

#if defined(LTM_ALL)
#define BN_ERROR_C
#define BN_FAST_MP_INVMOD_C
#define BN_FAST_MP_MONTGOMERY_REDUCE_C
#define BN_FAST_S_MP_MUL_DIGS_C
#define BN_FAST_S_MP_MUL_HIGH_DIGS_C
#define BN_FAST_S_MP_SQR_C
#define BN_MP_2EXPT_C
#define BN_MP_ABS_C
#define BN_MP_ADD_C
#define BN_MP_ADD_D_C
#define BN_MP_ADD_FACTORED_FACTORIALS_C
#define BN_MP_ADDMOD_C
#define BN_MP_AND_C
#define BN_MP_BALANCE_MUL_C
#define BN_MP_BELL_C
#define BN_MP_BELL_FAST_C
#define BN_MP_BINOMIAL_C
#define BN_MP_CATALAN_C
#define BN_MP_CLAMP_C
#define BN_MP_CLEAR_C
#define BN_MP_CLEAR_MULTI_C
#define BN_MP_CMP_C
#define BN_MP_CMP_D_C
#define BN_MP_CMP_MAG_C
#define BN_MP_CNT_LSB_C
#define BN_MP_COMPUTE_FACTORED_FACTORIAL_C
#define BN_MP_COMPUTE_SIGNED_FACTORED_FACTORIALS_C
#define BN_MP_COPY_C
#define BN_MP_COUNT_BITS_C
#define BN_MP_DIV_C
#define BN_MP_DIV_2_C
#define BN_MP_DIV_2D_C
#define BN_MP_DIV_3_C
#define BN_MP_DIV_D_C
#define BN_MP_DOUBLEFACTORIAL_C
#define BN_MP_DR_IS_MODULUS_C
#define BN_MP_DR_REDUCE_C
#define BN_MP_DR_SETUP_C
#define BN_MP_EULER_C
#define BN_MP_EXCH_C
#define BN_MP_EXPT_C
#define BN_MP_EXPT_D_C
#define BN_MP_EXPTMOD_C
#define BN_MP_EXPTMOD_FAST_C
#define BN_MP_EXTEUCLID_C
#define BN_MP_FACTOR_FACTORIAL_C
#define BN_MP_FACTORIAL_C
#define BN_MP_FALLING_FACTORIAL_C
#define BN_MP_FFT_C
#define BN_MP_FFT_MUL_C
#define BN_MP_FFT_SQR_C
#define BN_MP_FIBONACCI_C
#define BN_MP_FPL_MATRIX_C
#define BN_MP_FREAD_C
#define BN_MP_FWRITE_C
#define BN_MP_GCD_C
#define BN_MP_GET_INT_C
#define BN_MP_GROW_C
#define BN_MP_HIGHBIT_C
#define BN_MP_INIT_C
#define BN_MP_INIT_COPY_C
#define BN_MP_INIT_MULTI_C
#define BN_MP_INIT_SET_C
#define BN_MP_INIT_SET_INT_C
#define BN_MP_INIT_SIZE_C
#define BN_MP_INVMOD_C
#define BN_MP_INVMOD_SLOW_C
#define BN_MP_IS_SQUARE_C
#define BN_MP_ISDIVISIBLE_C
#define BN_MP_ISDIVISIBLE_D_C
#define BN_MP_ISGE32B_C
#define BN_MP_ISPERFPOWER_C
#define BN_MP_JACOBI_C
#define BN_MP_JACOBSTHAL_C
#define BN_MP_JACOBSTHAL_LUCAS_C
#define BN_MP_KARATSUBA_MUL_C
#define BN_MP_KARATSUBA_SQR_C
#define BN_MP_LCM_C
#define BN_MP_LEONARDO_C
#define BN_MP_LOWBIT_C
#define BN_MP_LSHD_C
#define BN_MP_LUCAS_C
#define BN_MP_MAX_C
#define BN_MP_MIN_C
#define BN_MP_MOD_C
#define BN_MP_MOD_2D_C
#define BN_MP_MOD_D_C
#define BN_MP_MONTGOMERY_CALC_NORMALIZATION_C
#define BN_MP_MONTGOMERY_REDUCE_C
#define BN_MP_MONTGOMERY_SETUP_C
#define BN_MP_MUL_C
#define BN_MP_MUL_2_C
#define BN_MP_MUL_2D_C
#define BN_MP_MUL_D_C
#define BN_MP_MULMOD_C
#define BN_MP_N_ROOT_C
#define BN_MP_NEG_C
#define BN_MP_NEGATE_FACTORED_FACTORIALS_C
#define BN_MP_OR_C
#define BN_MP_PADOVAN_C
#define BN_MP_PELL_C
#define BN_MP_PELL_LUCAS_C
#define BN_MP_PELL_MODIFIED_C
#define BN_MP_PERRIN_C
#define BN_MP_POWER_FACTORED_FACTORIALS_C
#define BN_MP_PRIME_FERMAT_C
#define BN_MP_PRIME_IS_DIVISIBLE_C
#define BN_MP_PRIME_IS_PRIME_C
#define BN_MP_PRIME_MILLER_RABIN_C
#define BN_MP_PRIME_NEXT_PRIME_C
#define BN_MP_PRIME_RABIN_MILLER_TRIALS_C
#define BN_MP_PRIME_RANDOM_EX_C
#define BN_MP_PRIMORIAL_C
#define BN_MP_RADIX_SIZE_C
#define BN_MP_RADIX_SMAP_C
#define BN_MP_RAND_C
#define BN_MP_READ_RADIX_C
#define BN_MP_READ_SIGNED_BIN_C
#define BN_MP_READ_UNSIGNED_BIN_C
#define BN_MP_REDUCE_C
#define BN_MP_REDUCE_2K_C
#define BN_MP_REDUCE_2K_L_C
#define BN_MP_REDUCE_2K_SETUP_C
#define BN_MP_REDUCE_2K_SETUP_L_C
#define BN_MP_REDUCE_IS_2K_C
#define BN_MP_REDUCE_IS_2K_L_C
#define BN_MP_REDUCE_SETUP_C
#define BN_MP_RISING_FACTORIAL_C
#define BN_MP_RSHD_C
#define BN_MP_SET_C
#define BN_MP_SET_INT_C
#define BN_MP_SET_WORD_C
#define BN_MP_SHRINK_C
#define BN_MP_SIGNED_BIN_SIZE_C
#define BN_MP_SQR_C
#define BN_MP_SQRMOD_C
#define BN_MP_SQRT_C
#define BN_MP_STIRLING1_C
#define BN_MP_STIRLING1_MAT_C
#define BN_MP_STIRLING2_C
#define BN_MP_STIRLING2_MAT_C
#define BN_MP_SUB_C
#define BN_MP_SUB_D_C
#define BN_MP_SUBFACTORIAL_C
#define BN_MP_SUBMOD_C
#define BN_MP_SUBTRACT_FACTORED_FACTORIALS_C
#define BN_MP_SUPERFACTORIAL_C
#define BN_MP_TO_SIGNED_BIN_C
#define BN_MP_TO_SIGNED_BIN_N_C
#define BN_MP_TO_UNSIGNED_BIN_C
#define BN_MP_TO_UNSIGNED_BIN_N_C
#define BN_MP_TOOM_COOK_4_MUL_C
#define BN_MP_TOOM_COOK_4_SQR_C
#define BN_MP_TOOM_COOK_5_MUL_C
#define BN_MP_TOOM_COOK_5_SQR_C
#define BN_MP_TOOM_MUL_C
#define BN_MP_TOOM_SQR_C
#define BN_MP_TORADIX_C
#define BN_MP_TORADIX_N_C
#define BN_MP_UNSIGNED_BIN_SIZE_C
#define BN_MP_XOR_C
#define BN_MP_ZERO_C
#define BN_PRIME_TAB_C
#define BN_REVERSE_C
#define BN_S_MP_ADD_C
#define BN_S_MP_EXPTMOD_C
#define BN_S_MP_MUL_DIGS_C
#define BN_S_MP_MUL_HIGH_DIGS_C
#define BN_S_MP_SQR_C
#define BN_S_MP_SUB_C
#define BNCORE_C
#endif

#if defined(BN_ERROR_C)
   #define BN_MP_ERROR_TO_STRING_C
#endif

#if defined(BN_FAST_MP_INVMOD_C)
   #define BN_MP_ISEVEN_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_COPY_C
   #define BN_MP_MOD_C
   #define BN_MP_SET_C
   #define BN_MP_DIV_2_C
   #define BN_MP_ISODD_C
   #define BN_MP_SUB_C
   #define BN_MP_CMP_C
   #define BN_MP_ISZERO_C
   #define BN_MP_CMP_D_C
   #define BN_MP_ADD_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_FAST_MP_MONTGOMERY_REDUCE_C)
   #define BN_MP_GROW_C
   #define BN_MP_RSHD_C
   #define BN_MP_CLAMP_C
   #define BN_MP_CMP_MAG_C
   #define BN_S_MP_SUB_C
#endif

#if defined(BN_FAST_S_MP_MUL_DIGS_C)
   #define BN_MP_GROW_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BN_FAST_S_MP_MUL_HIGH_DIGS_C)
   #define BN_MP_GROW_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BN_FAST_S_MP_SQR_C)
   #define BN_MP_GROW_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BN_MP_2EXPT_C)
   #define BN_MP_ZERO_C
   #define BN_MP_GROW_C
#endif

#if defined(BN_MP_ABS_C)
   #define BN_MP_COPY_C
#endif

#if defined(BN_MP_ADD_C)
   #define BN_S_MP_ADD_C
   #define BN_MP_CMP_MAG_C
   #define BN_S_MP_SUB_C
#endif

#if defined(BN_MP_ADD_D_C)
   #define BN_MP_GROW_C
   #define BN_MP_SUB_D_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BN_MP_ADD_FACTORED_FACTORIALS_C)
   #define BN_MP_SUBTRACT_FACTORED_FACTORIALS_C
#endif

#if defined(BN_MP_ADDMOD_C)
   #define BN_MP_INIT_C
   #define BN_MP_ADD_C
   #define BN_MP_CLEAR_C
   #define BN_MP_MOD_C
#endif

#if defined(BN_MP_AND_C)
   #define BN_MP_INIT_COPY_C
   #define BN_MP_CLAMP_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_BALANCE_MUL_C)
   #define BN_MP_MUL_C
   #define BN_MP_EXCH_C
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_LSHD_C
   #define BN_MP_ADD_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_BELL_C)
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_STIRLING2_C
   #define BN_MP_ADD_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_BELL_FAST_C)
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_STIRLING2_MAT_C
   #define BN_MP_ADD_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_MULTI_C
   #define BN_MP_STIRLING2_FREE_C
#endif

#if defined(BN_MP_BINOMIAL_C)
   #define BN_MP_COMB_C
   #define BN_MP_SET_INT_C
   #define BN_MP_INIT_C
   #define BN_MP_MUL_C
   #define BN_MP_DIV_C
   #define BN_MP_CLEAR_C
   #define BN_MP_BITSET_T_C
   #define BN_MP_BITSET_ALLOC_C
   #define BN_MP_ERATOSTHENES_C
   #define BN_MP_PRIME_DIVISORS_C
   #define BN_MP_BITSET_NEXTSET_C
   #define BN_MP_COMPUTE_FACTORED_FACTORIAL_C
#endif

#if defined(BN_MP_CATALAN_C)
   #define BN_MP_SET_INT_C
   #define BN_MP_BINOMIAL_C
   #define BN_MP_INIT_C
   #define BN_MP_DIV_C
   #define BN_MP_CLEAR_C
   #define BN_MP_DIV_D_C
#endif

#if defined(BN_MP_CLAMP_C)
#endif

#if defined(BN_MP_CLEAR_C)
#endif

#if defined(BN_MP_CLEAR_MULTI_C)
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_CMP_C)
   #define BN_MP_CMP_MAG_C
#endif

#if defined(BN_MP_CMP_D_C)
#endif

#if defined(BN_MP_CMP_MAG_C)
#endif

#if defined(BN_MP_CNT_LSB_C)
   #define BN_MP_ISZERO_C
#endif

#if defined(BN_MP_COMPUTE_FACTORED_FACTORIAL_C)
   #define BN_MP_SET_INT_C
   #define BN_MP_INIT_C
   #define BN_MP_SQR_C
   #define BN_MP_MUL_D_C
   #define BN_MP_MUL_C
   #define BN_MP_MUL_2D_C
#endif

#if defined(BN_MP_COMPUTE_SIGNED_FACTORED_FACTORIALS_C)
   #define BN_MP_COMPUTE_FACTORED_FACTORIALS_C
   #define BN_MP_SET_INT_C
   #define BN_MP_MUL_D_C
   #define BN_MP_SQR_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_INIT_C
   #define BN_MP_DIV_C
   #define BN_MP_COPY_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_COPY_C)
   #define BN_MP_GROW_C
#endif

#if defined(BN_MP_COUNT_BITS_C)
#endif

#if defined(BN_MP_DIV_C)
   #define BN_MP_ISZERO_C
   #define BN_MP_CMP_MAG_C
   #define BN_MP_COPY_C
   #define BN_MP_ZERO_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_SET_C
   #define BN_MP_COUNT_BITS_C
   #define BN_MP_ABS_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_CMP_C
   #define BN_MP_SUB_C
   #define BN_MP_ADD_C
   #define BN_MP_DIV_2D_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_MULTI_C
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_INIT_C
   #define BN_MP_INIT_COPY_C
   #define BN_MP_LSHD_C
   #define BN_MP_RSHD_C
   #define BN_MP_MUL_D_C
   #define BN_MP_CLAMP_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_DIV_2_C)
   #define BN_MP_GROW_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BN_MP_DIV_2D_C)
   #define BN_MP_COPY_C
   #define BN_MP_ZERO_C
   #define BN_MP_INIT_C
   #define BN_MP_MOD_2D_C
   #define BN_MP_CLEAR_C
   #define BN_MP_RSHD_C
   #define BN_MP_CLAMP_C
   #define BN_MP_EXCH_C
#endif

#if defined(BN_MP_DIV_3_C)
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_CLAMP_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_DIV_D_C)
   #define BN_MP_ISZERO_C
   #define BN_MP_COPY_C
   #define BN_MP_DIV_2D_C
   #define BN_MP_DIV_3_C
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_CLAMP_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_DOUBLEFACTORIAL_C)
   #define BN_MP_BITSET_T_C
   #define BN_MP_SET_C
   #define BN_MP_BITSET_ALLOC_C
   #define BN_MP_ERATOSTHENES_C
   #define BN_MP_PRIME_DIVISORS_C
   #define BN_MP_BITSET_NEXTSET_C
   #define BN_MP_COMPUTE_FACTORED_FACTORIAL_C
   #define BN_MP_FACTORIAL_C
   #define BN_MP_MUL_2D_C
#endif

#if defined(BN_MP_DR_IS_MODULUS_C)
#endif

#if defined(BN_MP_DR_REDUCE_C)
   #define BN_MP_GROW_C
   #define BN_MP_CLAMP_C
   #define BN_MP_CMP_MAG_C
   #define BN_S_MP_SUB_C
#endif

#if defined(BN_MP_DR_SETUP_C)
#endif

#if defined(BN_MP_EULER_C)
   #define BN_MP_INIT_C
   #define BN_MP_SET_INT_C
   #define BN_MP_MUL_D_C
   #define BN_MP_ADD_C
   #define BN_MP_CLEAR_C
   #define BN_MP_COPY_C
   #define BN_MP_EULER_FREE_C
#endif

#if defined(BN_MP_EXCH_C)
#endif

#if defined(BN_MP_EXPT_C)
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_COPY_C
   #define BN_MP_SET_INT_C
   #define BN_MP_ISZERO_C
   #define BN_MP_ISODD_C
   #define BN_MP_MUL_C
   #define BN_MP_DIV_2_C
   #define BN_MP_SQR_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_EXPT_D_C)
   #define BN_MP_INIT_COPY_C
   #define BN_MP_SET_C
   #define BN_MP_MUL_C
   #define BN_MP_CLEAR_C
   #define BN_MP_SQR_C
#endif

#if defined(BN_MP_EXPTMOD_C)
   #define BN_MP_INIT_C
   #define BN_MP_INVMOD_C
   #define BN_MP_CLEAR_C
   #define BN_MP_ABS_C
   #define BN_MP_CLEAR_MULTI_C
   #define BN_MP_REDUCE_IS_2K_L_C
   #define BN_S_MP_EXPTMOD_C
   #define BN_MP_DR_IS_MODULUS_C
   #define BN_MP_REDUCE_IS_2K_C
   #define BN_MP_ISODD_C
   #define BN_MP_EXPTMOD_FAST_C
#endif

#if defined(BN_MP_EXPTMOD_FAST_C)
   #define BN_MP_COUNT_BITS_C
   #define BN_MP_INIT_C
   #define BN_MP_CLEAR_C
   #define BN_MP_MONTGOMERY_SETUP_C
   #define BN_FAST_MP_MONTGOMERY_REDUCE_C
   #define BN_MP_MONTGOMERY_REDUCE_C
   #define BN_MP_DR_SETUP_C
   #define BN_MP_DR_REDUCE_C
   #define BN_MP_REDUCE_2K_SETUP_C
   #define BN_MP_REDUCE_2K_C
   #define BN_MP_MONTGOMERY_CALC_NORMALIZATION_C
   #define BN_MP_MULMOD_C
   #define BN_MP_SET_C
   #define BN_MP_MOD_C
   #define BN_MP_COPY_C
   #define BN_MP_SQR_C
   #define BN_MP_MUL_C
   #define BN_MP_EXCH_C
#endif

#if defined(BN_MP_EXTEUCLID_C)
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_SET_C
   #define BN_MP_COPY_C
   #define BN_MP_ISZERO_C
   #define BN_MP_DIV_C
   #define BN_MP_MUL_C
   #define BN_MP_SUB_C
   #define BN_MP_NEG_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_FACTOR_FACTORIAL_C)
   #define BN_MP_BITSET_T_C
   #define BN_MP_BITSET_ALLOC_C
   #define BN_MP_ERATOSTHENES_C
   #define BN_MP_PRIME_DIVISORS_C
   #define BN_MP_BITSET_NEXTSET_C
#endif

#if defined(BN_MP_FACTORIAL_C)
   #define BN_MP_PRIME_DIVISORS_C
   #define BN_MP_FILL_PRIME_LIST_C
   #define BN_MP_SET_INT_C
   #define BN_MP_INIT_C
   #define BN_MP_SQR_C
   #define BN_MP_MUL_D_C
   #define BN_MP_MUL_C
   #define BN_MP_PRIMORIAL_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_COPY_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_FALLING_FACTORIAL_C)
   #define BN_MP_BITSET_T_C
   #define BN_MP_SET_C
   #define BN_MP_SET_INT_C
   #define BN_MP_FACTORIAL_C
   #define BN_MP_BITSET_ALLOC_C
   #define BN_MP_ERATOSTHENES_C
   #define BN_MP_PRIME_DIVISORS_C
   #define BN_MP_BITSET_NEXTSET_C
   #define BN_MP_COMPUTE_FACTORED_FACTORIAL_C
#endif

#if defined(BN_MP_FFT_C)
   #define BN_MP_DP_TO_FFT_C
   #define BN_MP_DP_TO_FFT_SINGLE_C
   #define BN_MP_FFT_TO_DP_C
   #define BN_MP_GROW_C
   #define BN_MP_CLAMP_C
   #define BN_MP_FFT_SQR_D_C
#endif

#if defined(BN_MP_FFT_MUL_C)
   #define BN_MP_ISZERO_C
   #define BN_MP_COPY_C
   #define BN_MP_TOOM_COOK_5_MUL_C
   #define BN_MP_DP_TO_FFT_C
   #define BN_MP_FFT_C
   #define BN_MP_FFT_TO_DP_C
#endif

#if defined(BN_MP_FFT_SQR_C)
   #define BN_MP_ISZERO_C
   #define BN_MP_SET_INT_C
   #define BN_MP_TOOM_COOK_5_SQR_C
   #define BN_MP_DP_TO_FFT_SINGLE_C
   #define BN_MP_FFT_SQR_D_C
   #define BN_MP_FFT_TO_DP_C
#endif

#if defined(BN_MP_FIBONACCI_C)
   #define BN_MP_SET_INT_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_SET_C
   #define BN_MP_MUL_C
   #define BN_MP_ADD_C
   #define BN_MP_COPY_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_SQR_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_FPL_MATRIX_C)
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_SET_C
   #define BN_MP_MUL_C
   #define BN_MP_ADD_C
   #define BN_MP_COPY_C
   #define BN_MP_CLEAR_MULTI_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_FREAD_C)
   #define BN_MP_ZERO_C
   #define BN_MP_S_RMAP_C
   #define BN_MP_MUL_D_C
   #define BN_MP_ADD_D_C
   #define BN_MP_CMP_D_C
#endif

#if defined(BN_MP_FWRITE_C)
   #define BN_MP_RADIX_SIZE_C
   #define BN_MP_TORADIX_C
#endif

#if defined(BN_MP_GCD_C)
   #define BN_MP_ISZERO_C
   #define BN_MP_ABS_C
   #define BN_MP_INIT_COPY_C
   #define BN_MP_CNT_LSB_C
   #define BN_MP_DIV_2D_C
   #define BN_MP_CMP_MAG_C
   #define BN_MP_EXCH_C
   #define BN_S_MP_SUB_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_GET_INT_C)
#endif

#if defined(BN_MP_GROW_C)
#endif

#if defined(BN_MP_HIGHBIT_C)
   #define BN_MP_COUNT_BITS_C
#endif

#if defined(BN_MP_INIT_C)
#endif

#if defined(BN_MP_INIT_COPY_C)
   #define BN_MP_COPY_C
#endif

#if defined(BN_MP_INIT_MULTI_C)
   #define BN_MP_ERR_C
   #define BN_MP_INIT_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_INIT_SET_C)
   #define BN_MP_INIT_C
   #define BN_MP_SET_C
#endif

#if defined(BN_MP_INIT_SET_INT_C)
   #define BN_MP_INIT_C
   #define BN_MP_SET_INT_C
#endif

#if defined(BN_MP_INIT_SIZE_C)
   #define BN_MP_INIT_C
#endif

#if defined(BN_MP_INVMOD_C)
   #define BN_MP_ISZERO_C
   #define BN_MP_ISODD_C
   #define BN_FAST_MP_INVMOD_C
   #define BN_MP_INVMOD_SLOW_C
#endif

#if defined(BN_MP_INVMOD_SLOW_C)
   #define BN_MP_ISZERO_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_MOD_C
   #define BN_MP_COPY_C
   #define BN_MP_ISEVEN_C
   #define BN_MP_SET_C
   #define BN_MP_DIV_2_C
   #define BN_MP_ISODD_C
   #define BN_MP_ADD_C
   #define BN_MP_SUB_C
   #define BN_MP_CMP_C
   #define BN_MP_CMP_D_C
   #define BN_MP_CMP_MAG_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_IS_SQUARE_C)
   #define BN_MP_MOD_D_C
   #define BN_MP_INIT_SET_INT_C
   #define BN_MP_MOD_C
   #define BN_MP_GET_INT_C
   #define BN_MP_SQRT_C
   #define BN_MP_SQR_C
   #define BN_MP_CMP_MAG_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_ISDIVISIBLE_C)
   #define BN_MP_INIT_C
   #define BN_MP_MOD_C
   #define BN_MP_ISZERO_C
#endif

#if defined(BN_MP_ISDIVISIBLE_D_C)
   #define BN_MP_MOD_D_C
#endif

#if defined(BN_MP_ISGE32B_C)
#endif

#if defined(BN_MP_ISPERFPOWER_C)
   #define BN_MP_IS_SQUARE_C
   #define BN_MP_ISSQUARE_C
   #define BN_MP_SET_INT_C
   #define BN_MP_MOD_D_C
   #define BN_MP_INIT_SET_INT_C
   #define BN_MP_MOD_C
   #define BN_MP_GET_INT_C
   #define BN_MP_SQRT_C
   #define BN_MP_INIT_C
   #define BN_MP_SQR_C
   #define BN_MP_CMP_MAG_C
   #define BN_MP_CLEAR_C
   #define BN_MP_BITSET_T_C
   #define BN_MP_HIGHBIT_C
   #define BN_MP_LOWBIT_C
   #define BN_MP_IS_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_BITSET_ALLOC_C
   #define BN_MP_ERATOSTHENES_C
   #define BN_MP_BITSET_NEXTSET_C
   #define BN_MP_N_ROOT_C
   #define BN_MP_COPY_C
   #define BN_MP_EXPT_D_C
   #define BN_MP_CMP_C
   #define BN_MP_CLEAR_MULTI_C
   #define BN_MP_BITSET_FREE_C
#endif

#if defined(BN_MP_JACOBI_C)
   #define BN_MP_CMP_D_C
   #define BN_MP_ISZERO_C
   #define BN_MP_INIT_COPY_C
   #define BN_MP_CNT_LSB_C
   #define BN_MP_DIV_2D_C
   #define BN_MP_MOD_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_JACOBSTHAL_C)
   #define BN_MP_INIT_C
   #define BN_MP_SET_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_ADD_D_C
   #define BN_MP_DIV_D_C
   #define BN_MP_SUB_D_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_JACOBSTHAL_LUCAS_C)
   #define BN_MP_INIT_C
   #define BN_MP_SET_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_SUB_D_C
   #define BN_MP_ADD_D_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_KARATSUBA_MUL_C)
   #define BN_MP_MUL_C
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_CLAMP_C
   #define BN_S_MP_ADD_C
   #define BN_MP_ADD_C
   #define BN_S_MP_SUB_C
   #define BN_MP_LSHD_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_KARATSUBA_SQR_C)
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_CLAMP_C
   #define BN_MP_SQR_C
   #define BN_S_MP_ADD_C
   #define BN_S_MP_SUB_C
   #define BN_MP_LSHD_C
   #define BN_MP_ADD_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_LCM_C)
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_GCD_C
   #define BN_MP_CMP_MAG_C
   #define BN_MP_DIV_C
   #define BN_MP_MUL_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_LEONARDO_C)
   #define BN_MP_INIT_C
   #define BN_MP_FIBONACCI_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_SUB_D_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_LOWBIT_C)
#endif

#if defined(BN_MP_LSHD_C)
   #define BN_MP_GROW_C
   #define BN_MP_RSHD_C
#endif

#if defined(BN_MP_LUCAS_C)
   #define BN_MP_SET_INT_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_FIBONACCI_C
   #define BN_MP_ADD_C
#endif

#if defined(BN_MP_MAX_C)
   #define BN_MP_CMP_C
#endif

#if defined(BN_MP_MIN_C)
   #define BN_MP_CMP_C
#endif

#if defined(BN_MP_MOD_C)
   #define BN_MP_INIT_C
   #define BN_MP_DIV_C
   #define BN_MP_CLEAR_C
   #define BN_MP_ISZERO_C
   #define BN_MP_EXCH_C
   #define BN_MP_ADD_C
#endif

#if defined(BN_MP_MOD_2D_C)
   #define BN_MP_ZERO_C
   #define BN_MP_COPY_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BN_MP_MOD_D_C)
   #define BN_MP_DIV_D_C
#endif

#if defined(BN_MP_MONTGOMERY_CALC_NORMALIZATION_C)
   #define BN_MP_COUNT_BITS_C
   #define BN_MP_2EXPT_C
   #define BN_MP_SET_C
   #define BN_MP_MUL_2_C
   #define BN_MP_CMP_MAG_C
   #define BN_S_MP_SUB_C
#endif

#if defined(BN_MP_MONTGOMERY_REDUCE_C)
   #define BN_FAST_MP_MONTGOMERY_REDUCE_C
   #define BN_MP_GROW_C
   #define BN_MP_CLAMP_C
   #define BN_MP_RSHD_C
   #define BN_MP_CMP_MAG_C
   #define BN_S_MP_SUB_C
#endif

#if defined(BN_MP_MONTGOMERY_SETUP_C)
#endif

#if defined(BN_MP_MUL_C)
   #define BN_MP_EXCH_C
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_LSHD_C
   #define BN_MP_ADD_C
   #define BN_MP_CLEAR_MULTI_C
   #define BN_MP_FFT_MUL_C
   #define BN_MP_TOOM_COOK_5_MUL_C
   #define BN_MP_TOOM_COOK_4_MUL_C
   #define BN_MP_TOOM_MUL_C
   #define BN_MP_KARATSUBA_MUL_C
   #define BN_FAST_S_MP_MUL_DIGS_C
   #define BN_S_MP_MUL_C
   #define BN_S_MP_MUL_DIGS_C
#endif

#if defined(BN_MP_MUL_2_C)
   #define BN_MP_GROW_C
#endif

#if defined(BN_MP_MUL_2D_C)
   #define BN_MP_COPY_C
   #define BN_MP_GROW_C
   #define BN_MP_LSHD_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BN_MP_MUL_D_C)
   #define BN_MP_GROW_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BN_MP_MULMOD_C)
   #define BN_MP_INIT_C
   #define BN_MP_MUL_C
   #define BN_MP_CLEAR_C
   #define BN_MP_MOD_C
#endif

#if defined(BN_MP_N_ROOT_C)
   #define BN_MP_INIT_C
   #define BN_MP_COUNT_BITS_C
   #define BN_MP_2EXPT_C
   #define BN_MP_COPY_C
   #define BN_MP_EXPT_D_C
   #define BN_MP_MUL_C
   #define BN_MP_SUB_C
   #define BN_MP_MUL_D_C
   #define BN_MP_DIV_C
   #define BN_MP_CMP_C
   #define BN_MP_SUB_D_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_NEG_C)
   #define BN_MP_COPY_C
   #define BN_MP_ISZERO_C
#endif

#if defined(BN_MP_NEGATE_FACTORED_FACTORIALS_C)
#endif

#if defined(BN_MP_OR_C)
   #define BN_MP_INIT_COPY_C
   #define BN_MP_CLAMP_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_PADOVAN_C)
   #define BN_MP_SET_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_MUL_C
   #define BN_MP_ADD_C
   #define BN_MP_COPY_C
   #define BN_MP_CLEAR_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_PELL_C)
   #define BN_MP_FPL_MATRIX_C
#endif

#if defined(BN_MP_PELL_LUCAS_C)
   #define BN_MP_FPL_MATRIX_C
#endif

#if defined(BN_MP_PELL_MODIFIED_C)
   #define BN_MP_FPL_MATRIX_C
#endif

#if defined(BN_MP_PERRIN_C)
   #define BN_MP_SET_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_MUL_C
   #define BN_MP_ADD_C
   #define BN_MP_COPY_C
   #define BN_MP_FWRITE_C
   #define BN_MP_CLEAR_MULTI_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_POWER_FACTORED_FACTORIALS_C)
#endif

#if defined(BN_MP_PRIME_FERMAT_C)
   #define BN_MP_CMP_D_C
   #define BN_MP_INIT_C
   #define BN_MP_EXPTMOD_C
   #define BN_MP_CMP_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_PRIME_IS_DIVISIBLE_C)
   #define BN_MP_MOD_D_C
#endif

#if defined(BN_MP_PRIME_IS_PRIME_C)
   #define BN_MP_CMP_D_C
   #define BN_MP_PRIME_IS_DIVISIBLE_C
   #define BN_MP_INIT_C
   #define BN_MP_SET_C
   #define BN_MP_PRIME_MILLER_RABIN_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_PRIME_MILLER_RABIN_C)
   #define BN_MP_CMP_D_C
   #define BN_MP_INIT_COPY_C
   #define BN_MP_SUB_D_C
   #define BN_MP_CNT_LSB_C
   #define BN_MP_DIV_2D_C
   #define BN_MP_EXPTMOD_C
   #define BN_MP_CMP_C
   #define BN_MP_SQRMOD_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_PRIME_NEXT_PRIME_C)
   #define BN_MP_CMP_D_C
   #define BN_MP_SET_C
   #define BN_MP_SUB_D_C
   #define BN_MP_ISEVEN_C
   #define BN_MP_MOD_D_C
   #define BN_MP_INIT_C
   #define BN_MP_ADD_D_C
   #define BN_MP_PRIME_MILLER_RABIN_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_PRIME_RABIN_MILLER_TRIALS_C)
#endif

#if defined(BN_MP_PRIME_RANDOM_EX_C)
   #define BN_MP_READ_UNSIGNED_BIN_C
   #define BN_MP_PRIME_IS_PRIME_C
   #define BN_MP_SUB_D_C
   #define BN_MP_DIV_2_C
   #define BN_MP_MUL_2_C
   #define BN_MP_ADD_D_C
#endif

#if defined(BN_MP_PRIMORIAL_C)
   #define BN_MP_BITSET_NEXTSET_C
   #define BN_MP_BITSET_T_C
   #define BN_MP_BITSET_SIZE_C
   #define BN_MP_BITSET_GET_C
   #define BN_MP_BITSET_PREVSET_C
   #define BN_MP_ERATOSTHENES_C
   #define BN_MP_BITSET_SETALL_C
   #define BN_MP_BITSET_CLEAR_C
   #define BN_MP_FILL_PRIME_LIST_C
   #define BN_MP_BITSET_ALLOC_C
   #define BN_MP_BITSET_FREE_C
   #define BN_MP_SET_INT_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_COPY_C
   #define BN_MP_MUL_C
   #define BN_MP_CLEAR_MULTI_C
   #define BN_MP_MUL_D_C
   #define BN_MP_INIT_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_RADIX_SIZE_C)
   #define BN_MP_COUNT_BITS_C
   #define BN_MP_ISZERO_C
   #define BN_MP_INIT_COPY_C
   #define BN_MP_DIV_D_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_RADIX_SMAP_C)
   #define BN_MP_S_RMAP_C
#endif

#if defined(BN_MP_RAND_C)
   #define BN_MP_ZERO_C
   #define BN_MP_ADD_D_C
   #define BN_MP_LSHD_C
#endif

#if defined(BN_MP_READ_RADIX_C)
   #define BN_MP_ZERO_C
   #define BN_MP_S_RMAP_C
   #define BN_MP_MUL_D_C
   #define BN_MP_ADD_D_C
   #define BN_MP_ISZERO_C
#endif

#if defined(BN_MP_READ_SIGNED_BIN_C)
   #define BN_MP_READ_UNSIGNED_BIN_C
#endif

#if defined(BN_MP_READ_UNSIGNED_BIN_C)
   #define BN_MP_GROW_C
   #define BN_MP_ZERO_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BN_MP_REDUCE_C)
   #define BN_MP_REDUCE_SETUP_C
   #define BN_MP_INIT_COPY_C
   #define BN_MP_RSHD_C
   #define BN_MP_MUL_C
   #define BN_S_MP_MUL_HIGH_DIGS_C
   #define BN_FAST_S_MP_MUL_HIGH_DIGS_C
   #define BN_MP_MOD_2D_C
   #define BN_S_MP_MUL_DIGS_C
   #define BN_MP_SUB_C
   #define BN_MP_CMP_D_C
   #define BN_MP_SET_C
   #define BN_MP_LSHD_C
   #define BN_MP_ADD_C
   #define BN_MP_CMP_C
   #define BN_S_MP_SUB_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_REDUCE_2K_C)
   #define BN_MP_INIT_C
   #define BN_MP_COUNT_BITS_C
   #define BN_MP_DIV_2D_C
   #define BN_MP_MUL_D_C
   #define BN_S_MP_ADD_C
   #define BN_MP_CMP_MAG_C
   #define BN_S_MP_SUB_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_REDUCE_2K_L_C)
   #define BN_MP_INIT_C
   #define BN_MP_COUNT_BITS_C
   #define BN_MP_DIV_2D_C
   #define BN_MP_MUL_C
   #define BN_S_MP_ADD_C
   #define BN_MP_CMP_MAG_C
   #define BN_S_MP_SUB_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_REDUCE_2K_SETUP_C)
   #define BN_MP_INIT_C
   #define BN_MP_COUNT_BITS_C
   #define BN_MP_2EXPT_C
   #define BN_MP_CLEAR_C
   #define BN_S_MP_SUB_C
#endif

#if defined(BN_MP_REDUCE_2K_SETUP_L_C)
   #define BN_MP_INIT_C
   #define BN_MP_2EXPT_C
   #define BN_MP_COUNT_BITS_C
   #define BN_S_MP_SUB_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_REDUCE_IS_2K_C)
   #define BN_MP_REDUCE_2K_C
   #define BN_MP_COUNT_BITS_C
#endif

#if defined(BN_MP_REDUCE_IS_2K_L_C)
#endif

#if defined(BN_MP_REDUCE_SETUP_C)
   #define BN_MP_2EXPT_C
   #define BN_MP_DIV_C
#endif

#if defined(BN_MP_RISING_FACTORIAL_C)
   #define BN_MP_FALLING_FACTORIAL_C
#endif

#if defined(BN_MP_RSHD_C)
   #define BN_MP_ZERO_C
#endif

#if defined(BN_MP_SET_C)
   #define BN_MP_ZERO_C
#endif

#if defined(BN_MP_SET_INT_C)
   #define BN_MP_ZERO_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BN_MP_SET_WORD_C)
   #define BN_MP_ZERO_C
#endif

#if defined(BN_MP_SHRINK_C)
#endif

#if defined(BN_MP_SIGNED_BIN_SIZE_C)
   #define BN_MP_UNSIGNED_BIN_SIZE_C
#endif

#if defined(BN_MP_SQR_C)
   #define BN_MP_FFT_SQR_C
   #define BN_MP_TOOM_SQR_C
   #define BN_MP_KARATSUBA_SQR_C
   #define BN_FAST_S_MP_SQR_C
   #define BN_S_MP_SQR_C
#endif

#if defined(BN_MP_SQRMOD_C)
   #define BN_MP_INIT_C
   #define BN_MP_SQR_C
   #define BN_MP_CLEAR_C
   #define BN_MP_MOD_C
#endif

#if defined(BN_MP_SQRT_C)
   #define BN_MP_N_ROOT_C
   #define BN_MP_ISZERO_C
   #define BN_MP_ZERO_C
   #define BN_MP_INIT_COPY_C
   #define BN_MP_RSHD_C
   #define BN_MP_DIV_C
   #define BN_MP_ADD_C
   #define BN_MP_DIV_2_C
   #define BN_MP_CMP_MAG_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_STIRLING1_C)
   #define BN_MP_SET_C
   #define BN_MP_COPY_C
   #define BN_MP_FACTORIAL_C
   #define BN_MP_NEG_C
   #define BN_MP_BINOMIAL_C
   #define BN_MP_INIT_C
   #define BN_MP_SET_INT_C
   #define BN_MP_MUL_C
   #define BN_MP_SUB_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_STIRLING1_MAT_C)
   #define BN_MP_COPY_C
   #define BN_MP_CLEAR_C
   #define BN_MP_INIT_C
   #define BN_MP_SET_C
   #define BN_MP_SET_INT_C
   #define BN_MP_MUL_C
   #define BN_MP_SUB_C
   #define BN_MP_STIRLING1_FREE_C
#endif

#if defined(BN_MP_STIRLING2_C)
   #define BN_MP_SET_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_SET_INT_C
   #define BN_MP_EXPT_C
   #define BN_MP_SUB_D_C
   #define BN_MP_CLEAR_MULTI_C
   #define BN_MP_EXPT_D_C
   #define BN_MP_BINOMIAL_C
   #define BN_MP_MUL_C
   #define BN_MP_MUL_D_C
   #define BN_MP_DIV_C
   #define BN_MP_DIV_D_C
   #define BN_MP_SUB_C
   #define BN_MP_ADD_C
   #define BN_MP_FACTORIAL_C
   #define BN_MP_EXCH_C
#endif

#if defined(BN_MP_STIRLING2_MAT_C)
   #define BN_MP_COPY_C
   #define BN_MP_CLEAR_C
   #define BN_MP_INIT_C
   #define BN_MP_SET_C
   #define BN_MP_SET_INT_C
   #define BN_MP_MUL_C
   #define BN_MP_ADD_C
   #define BN_MP_STIRLING2_FREE_C
#endif

#if defined(BN_MP_SUB_C)
   #define BN_S_MP_ADD_C
   #define BN_MP_CMP_MAG_C
   #define BN_S_MP_SUB_C
#endif

#if defined(BN_MP_SUB_D_C)
   #define BN_MP_GROW_C
   #define BN_MP_ADD_D_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BN_MP_SUBFACTORIAL_C)
   #define BN_MP_SET_INT_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_CLEAR_MULTI_C
   #define BN_MP_EXCH_C
   #define BN_MP_ADD_C
   #define BN_MP_MUL_D_C
   #define BN_MP_ADD_D_C
   #define BN_MP_INIT_C
   #define BN_MP_COPY_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_SUBMOD_C)
   #define BN_MP_INIT_C
   #define BN_MP_SUB_C
   #define BN_MP_CLEAR_C
   #define BN_MP_MOD_C
#endif

#if defined(BN_MP_SUBTRACT_FACTORED_FACTORIALS_C)
#endif

#if defined(BN_MP_SUPERFACTORIAL_C)
   #define BN_MP_SET_C
   #define BN_MP_FACTOR_FACTORIAL_C
   #define BN_MP_ADD_FACTORED_FACTORIALS_C
   #define BN_MP_COMPUTE_SIGNED_FACTORED_FACTORIALS_C
#endif

#if defined(BN_MP_TO_SIGNED_BIN_C)
   #define BN_MP_TO_UNSIGNED_BIN_C
#endif

#if defined(BN_MP_TO_SIGNED_BIN_N_C)
   #define BN_MP_SIGNED_BIN_SIZE_C
   #define BN_MP_TO_SIGNED_BIN_C
#endif

#if defined(BN_MP_TO_UNSIGNED_BIN_C)
   #define BN_MP_INIT_COPY_C
   #define BN_MP_ISZERO_C
   #define BN_MP_DIV_2D_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_TO_UNSIGNED_BIN_N_C)
   #define BN_MP_UNSIGNED_BIN_SIZE_C
   #define BN_MP_TO_UNSIGNED_BIN_C
#endif

#if defined(BN_MP_TOOM_COOK_4_MUL_C)
   #define BN_MP_TOOM_MUL_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_COPY_C
   #define BN_MP_ADD_C
   #define BN_MP_SUB_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_DIV_2D_C
   #define BN_MP_MUL_D_C
   #define BN_MP_DIV_3_C
   #define BN_MP_DIV_D_C
   #define BN_MP_LSHD_C
   #define BN_MP_CLEAR_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_TOOM_COOK_4_SQR_C)
   #define BN_MP_TOOM_SQR_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_MUL_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_ADD_C
   #define BN_MP_SUB_C
   #define BN_MP_DIV_2D_C
   #define BN_MP_COPY_C
   #define BN_MP_LSHD_C
   #define BN_MP_CLEAR_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_TOOM_COOK_5_MUL_C)
   #define BN_MP_TOOM_COOK_4_MUL_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_SUB_C
   #define BN_MP_ADD_C
   #define BN_MP_DIV_2D_C
   #define BN_MP_NEG_C
   #define BN_MP_MUL_D_C
   #define BN_MP_DIV_D_C
   #define BN_MP_COPY_C
   #define BN_MP_LSHD_C
   #define BN_MP_CLEAR_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_TOOM_COOK_5_SQR_C)
   #define BN_MP_TOOM_COOK_4_SQR_C
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_ADD_C
   #define BN_MP_SUB_C
   #define BN_MP_MUL_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_DIV_2D_C
   #define BN_MP_COPY_C
   #define BN_MP_LSHD_C
   #define BN_MP_CLEAR_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_TOOM_MUL_C)
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_MOD_2D_C
   #define BN_MP_COPY_C
   #define BN_MP_RSHD_C
   #define BN_MP_MUL_C
   #define BN_MP_MUL_2_C
   #define BN_MP_ADD_C
   #define BN_MP_SUB_C
   #define BN_MP_DIV_2_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_MUL_D_C
   #define BN_MP_DIV_3_C
   #define BN_MP_LSHD_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_TOOM_SQR_C)
   #define BN_MP_INIT_MULTI_C
   #define BN_MP_MOD_2D_C
   #define BN_MP_COPY_C
   #define BN_MP_RSHD_C
   #define BN_MP_SQR_C
   #define BN_MP_MUL_2_C
   #define BN_MP_ADD_C
   #define BN_MP_SUB_C
   #define BN_MP_DIV_2_C
   #define BN_MP_MUL_2D_C
   #define BN_MP_MUL_D_C
   #define BN_MP_DIV_3_C
   #define BN_MP_LSHD_C
   #define BN_MP_CLEAR_MULTI_C
#endif

#if defined(BN_MP_TORADIX_C)
   #define BN_MP_ISZERO_C
   #define BN_MP_INIT_COPY_C
   #define BN_MP_DIV_D_C
   #define BN_MP_CLEAR_C
   #define BN_MP_S_RMAP_C
#endif

#if defined(BN_MP_TORADIX_N_C)
   #define BN_MP_ISZERO_C
   #define BN_MP_INIT_COPY_C
   #define BN_MP_DIV_D_C
   #define BN_MP_CLEAR_C
   #define BN_MP_S_RMAP_C
#endif

#if defined(BN_MP_UNSIGNED_BIN_SIZE_C)
   #define BN_MP_COUNT_BITS_C
#endif

#if defined(BN_MP_XOR_C)
   #define BN_MP_INIT_COPY_C
   #define BN_MP_CLAMP_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_MP_ZERO_C)
#endif

#if defined(BN_PRIME_TAB_C)
#endif

#if defined(BN_REVERSE_C)
#endif

#if defined(BN_S_MP_ADD_C)
   #define BN_MP_GROW_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BN_S_MP_EXPTMOD_C)
   #define BN_MP_COUNT_BITS_C
   #define BN_MP_INIT_C
   #define BN_MP_CLEAR_C
   #define BN_MP_REDUCE_SETUP_C
   #define BN_MP_REDUCE_C
   #define BN_MP_REDUCE_2K_SETUP_L_C
   #define BN_MP_REDUCE_2K_L_C
   #define BN_MP_MOD_C
   #define BN_MP_COPY_C
   #define BN_MP_SQR_C
   #define BN_MP_MUL_C
   #define BN_MP_SET_C
   #define BN_MP_EXCH_C
#endif

#if defined(BN_S_MP_MUL_DIGS_C)
   #define BN_FAST_S_MP_MUL_DIGS_C
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_CLAMP_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_S_MP_MUL_HIGH_DIGS_C)
   #define BN_FAST_S_MP_MUL_HIGH_DIGS_C
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_CLAMP_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_S_MP_SQR_C)
   #define BN_MP_INIT_SIZE_C
   #define BN_MP_CLAMP_C
   #define BN_MP_EXCH_C
   #define BN_MP_CLEAR_C
#endif

#if defined(BN_S_MP_SUB_C)
   #define BN_MP_GROW_C
   #define BN_MP_CLAMP_C
#endif

#if defined(BNCORE_C)
#endif

#ifdef LTM3
#define LTM_LAST
#endif
#include <tommath_superclass.h>
#include <tommath_class.h>
#else
#define LTM_LAST
#endif
