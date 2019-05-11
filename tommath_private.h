/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifndef TOMMATH_PRIV_H_
#define TOMMATH_PRIV_H_

#include "tommath.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Tunable cutoffs
 * ---------------
 *
 *  - In the default settings, a cutoff X can be modified at runtime
 *    by adjusting the corresponding X_CUTOFF variable.
 *
 *  - Tunability of the library can be disabled at compile time
 *    by defining the MP_FIXED_CUTOFFS macro.
 *
 *  - There is an additional file tommath_cutoffs.h, which defines
 *    the default cutoffs. These can be adjusted manually or by the
 *    autotuner.
 *
 */

#ifdef MP_FIXED_CUTOFFS
#  include "tommath_cutoffs.h"
#  define MP_KARATSUBA_MUL_CUTOFF MP_DEFAULT_KARATSUBA_MUL_CUTOFF
#  define MP_KARATSUBA_SQR_CUTOFF MP_DEFAULT_KARATSUBA_SQR_CUTOFF
#  define MP_TOOM_MUL_CUTOFF      MP_DEFAULT_TOOM_MUL_CUTOFF
#  define MP_TOOM_SQR_CUTOFF      MP_DEFAULT_TOOM_SQR_CUTOFF
#else
#  define MP_KARATSUBA_MUL_CUTOFF KARATSUBA_MUL_CUTOFF
#  define MP_KARATSUBA_SQR_CUTOFF KARATSUBA_SQR_CUTOFF
#  define MP_TOOM_MUL_CUTOFF      TOOM_MUL_CUTOFF
#  define MP_TOOM_SQR_CUTOFF      TOOM_SQR_CUTOFF
#endif

/* define heap macros */
#ifndef MP_MALLOC
/* default to libc stuff */
#   include <stdlib.h>
#   define MP_MALLOC(size)                   malloc(size)
#   define MP_REALLOC(mem, oldsize, newsize) realloc(mem, newsize)
#   define MP_CALLOC(nmemb, size)            calloc(nmemb, size)
#   define MP_FREE(mem, size)                free(mem)
#else
/* prototypes for our heap functions */
extern void *MP_MALLOC(size_t size);
extern void *MP_REALLOC(void *mem, size_t oldsize, size_t newsize);
extern void *MP_CALLOC(size_t nmemb, size_t size);
extern void MP_FREE(void *mem, size_t size);
#endif

/* TODO: Remove PRIVATE_MP_WARRAY as soon as deprecated MP_WARRAY is removed from tommath.h */
#undef MP_WARRAY
#define MP_WARRAY PRIVATE_MP_WARRAY

#define MP_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MP_MAX(x, y) (((x) > (y)) ? (x) : (y))

/* ---> Basic Manipulations <--- */
#define MP_IS_ZERO(a) ((a)->used == 0)
#define MP_IS_EVEN(a) (((a)->used == 0) || (((a)->dp[0] & 1u) == 0u))
#define MP_IS_ODD(a)  (((a)->used > 0) && (((a)->dp[0] & 1u) == 1u))

#define MP_SIZEOF_BITS(type)    (CHAR_BIT * sizeof(type))
#define MP_MAXFAST              (int)(1u << (MP_SIZEOF_BITS(mp_word) - (2u * (size_t)MP_DIGIT_BIT)))

/* random number source */
extern int (*s_mp_rand_source)(void *out, size_t size);

/* lowlevel functions, do not call! */
MP_WUR int s_mp_add(const mp_int *a, const mp_int *b, mp_int *c);
MP_WUR int s_mp_sub(const mp_int *a, const mp_int *b, mp_int *c);
MP_WUR int s_mp_mul_digs_fast(const mp_int *a, const mp_int *b, mp_int *c, int digs);
MP_WUR int s_mp_mul_digs(const mp_int *a, const mp_int *b, mp_int *c, int digs);
MP_WUR int s_mp_mul_high_digs_fast(const mp_int *a, const mp_int *b, mp_int *c, int digs);
MP_WUR int s_mp_mul_high_digs(const mp_int *a, const mp_int *b, mp_int *c, int digs);
MP_WUR int s_mp_sqr_fast(const mp_int *a, mp_int *b);
MP_WUR int s_mp_sqr(const mp_int *a, mp_int *b);
MP_WUR int s_mp_balance_mul(const mp_int *a, const mp_int *b, mp_int *c);
MP_WUR int s_mp_karatsuba_mul(const mp_int *a, const mp_int *b, mp_int *c);
MP_WUR int s_mp_toom_mul(const mp_int *a, const mp_int *b, mp_int *c);
MP_WUR int s_mp_karatsuba_sqr(const mp_int *a, mp_int *b);
MP_WUR int s_mp_toom_sqr(const mp_int *a, mp_int *b);
MP_WUR int s_mp_invmod_fast(const mp_int *a, const mp_int *b, mp_int *c);
MP_WUR int s_mp_invmod_slow(const mp_int *a, const mp_int *b, mp_int *c);
MP_WUR int s_mp_montgomery_reduce_fast(mp_int *x, const mp_int *n, mp_digit rho);
MP_WUR int s_mp_exptmod_fast(const mp_int *G, const mp_int *X, const mp_int *P, mp_int *Y, int redmode);
MP_WUR int s_mp_exptmod(const mp_int *G, const mp_int *X, const mp_int *P, mp_int *Y, int redmode);
void s_mp_reverse(unsigned char *s, int len);

extern const char *const mp_s_rmap;
extern const uint8_t mp_s_rmap_reverse[];
extern const size_t mp_s_rmap_reverse_sz;

/* Fancy macro to set an MPI from another type.
 * There are several things assumed:
 *  x is the counter
 *  a is the pointer to the MPI
 *  b is the original value that should be set in the MPI.
 */
#define MP_SET_XLONG(func_name, type)                    \
int func_name (mp_int * a, type b)                       \
{                                                        \
   int x = 0;                                            \
   int new_size = ((MP_SIZEOF_BITS(type) + MP_DIGIT_BIT) - 1) / MP_DIGIT_BIT; \
   int res = mp_grow(a, new_size);                       \
   if (res == MP_OKAY) {                                 \
     mp_zero(a);                                         \
     while (b != 0u) {                                   \
        a->dp[x++] = ((mp_digit)b & MP_MASK);            \
        if (MP_SIZEOF_BITS(b) <= MP_DIGIT_BIT) { break; } \
        b >>= ((MP_SIZEOF_BITS(b) <= MP_DIGIT_BIT) ? 0 : MP_DIGIT_BIT); \
     }                                                   \
     a->used = x;                                        \
   }                                                     \
   return res;                                           \
}

/* deprecated functions */
MP_DEPRECATED(s_mp_invmod_fast) int fast_mp_invmod(const mp_int *a, const mp_int *b, mp_int *c);
MP_DEPRECATED(s_mp_montgomery_reduce_fast) int fast_mp_montgomery_reduce(mp_int *x, const mp_int *n, mp_digit rho);
MP_DEPRECATED(s_mp_mul_digs_fast) int fast_s_mp_mul_digs(const mp_int *a, const mp_int *b, mp_int *c, int digs);
MP_DEPRECATED(s_mp_mul_high_digs_fast) int fast_s_mp_mul_high_digs(const mp_int *a, const mp_int *b, mp_int *c,
      int digs);
MP_DEPRECATED(s_mp_sqr_fast) int fast_s_mp_sqr(const mp_int *a, mp_int *b);
MP_DEPRECATED(s_mp_balance_mul) int mp_balance_mul(const mp_int *a, const mp_int *b, mp_int *c);
MP_DEPRECATED(s_mp_exptmod_fast) int mp_exptmod_fast(const mp_int *G, const mp_int *X, const mp_int *P, mp_int *Y,
      int redmode);
MP_DEPRECATED(s_mp_invmod_slow) int mp_invmod_slow(const mp_int *a, const mp_int *b, mp_int *c);
MP_DEPRECATED(s_mp_karatsuba_mul) int mp_karatsuba_mul(const mp_int *a, const mp_int *b, mp_int *c);
MP_DEPRECATED(s_mp_karatsuba_sqr) int mp_karatsuba_sqr(const mp_int *a, mp_int *b);
MP_DEPRECATED(s_mp_toom_mul) int mp_toom_mul(const mp_int *a, const mp_int *b, mp_int *c);
MP_DEPRECATED(s_mp_toom_sqr) int mp_toom_sqr(const mp_int *a, mp_int *b);
MP_DEPRECATED(s_mp_reverse) void bn_reverse(unsigned char *s, int len);

#ifdef __cplusplus
}
#endif

#endif
