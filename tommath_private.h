/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifndef TOMMATH_PRIV_H_
#define TOMMATH_PRIV_H_

#include "tommath.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Hardening libtommath
 * --------------------
 *
 * By default memory is zeroed before calling
 * MP_FREE to avoid leaking data. This is good
 * practice in cryptographical applications.
 *
 * Note however that memory allocators used
 * in cryptographical applications can often
 * be configured by itself to clear memory,
 * rendering the clearing in tommath unnecessary.
 * See for example https://github.com/GrapheneOS/hardened_malloc
 * and the option CONFIG_ZERO_ON_FREE.
 *
 * Furthermore there are applications which
 * value performance more and want this
 * feature to be disabled. For such applications
 * define MP_NO_ZERO_ON_FREE during compilation.
 */
#ifdef MP_NO_ZERO_ON_FREE
#  define MP_FREE_BUFFER(mem, size)   MP_FREE((mem), (size))
#  define MP_FREE_DIGITS(mem, digits) MP_FREE((mem), sizeof (mp_digit) * (size_t)(digits))
#else
#  define MP_FREE_BUFFER(mem, size)                     \
do {                                                    \
   size_t fs_ = (size);                                 \
   void* fm_ = (mem);                                   \
   if (fm_ != NULL) {                                   \
      MP_ZERO_BUFFER(fm_, fs_);                         \
      MP_FREE(fm_, fs_);                                \
   }                                                    \
} while (0)
#  define MP_FREE_DIGITS(mem, digits)                   \
do {                                                    \
   int fd_ = (digits);                                  \
   void* fm_ = (mem);                                   \
   if (fm_ != NULL) {                                   \
      size_t fs_ = sizeof (mp_digit) * (size_t)fd_;     \
      MP_ZERO_BUFFER(fm_, fs_);                         \
      MP_FREE(fm_, fs_);                                \
   }                                                    \
} while (0)
#endif

#ifdef MP_USE_MEMSET
#  include <string.h>
#  define MP_ZERO_BUFFER(mem, size)   memset((mem), 0, (size))
#  define MP_ZERO_DIGITS(mem, digits)                   \
do {                                                    \
   int zd_ = (digits);                                  \
   if (zd_ > 0) {                                       \
      memset((mem), 0, sizeof(mp_digit) * (size_t)zd_); \
   }                                                    \
} while (0)
#else
#  define MP_ZERO_BUFFER(mem, size)                     \
do {                                                    \
   size_t zs_ = (size);                                 \
   char* zm_ = (char*)(mem);                            \
   while (zs_-- > 0) {                                  \
      *zm_++ = 0;                                       \
   }                                                    \
} while (0)
#  define MP_ZERO_DIGITS(mem, digits)                   \
do {                                                    \
   int zd_ = (digits);                                  \
   mp_digit* zm_ = (mem);                               \
   while (zd_-- > 0) {                                  \
      *zm_++ = 0;                                       \
   }                                                    \
} while (0)
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
#   define MP_REALLOC(mem, oldsize, newsize) realloc((mem), (newsize))
#   define MP_CALLOC(nmemb, size)            calloc((nmemb), (size))
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
#define MP_MAXFAST              (int)(1uL << (MP_SIZEOF_BITS(mp_word) - (2u * (size_t)MP_DIGIT_BIT)))

/* random number source */
extern mp_err(*s_mp_rand_source)(void *out, size_t size);

/* Minimum number of available digits in mp_int, MP_PREC >= MP_MIN_PREC */
#define MP_MIN_PREC ((((CHAR_BIT * (int)sizeof(long long)) + MP_DIGIT_BIT) - 1) / MP_DIGIT_BIT)

/* lowlevel functions, do not call! */
mp_err s_mp_add(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;
mp_err s_mp_sub(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;
mp_err s_mp_mul_digs_fast(const mp_int *a, const mp_int *b, mp_int *c, int digs) MP_WUR;
mp_err s_mp_mul_digs(const mp_int *a, const mp_int *b, mp_int *c, int digs) MP_WUR;
mp_err s_mp_mul_high_digs_fast(const mp_int *a, const mp_int *b, mp_int *c, int digs) MP_WUR;
mp_err s_mp_mul_high_digs(const mp_int *a, const mp_int *b, mp_int *c, int digs) MP_WUR;
mp_err s_mp_sqr_fast(const mp_int *a, mp_int *b) MP_WUR;
mp_err s_mp_sqr(const mp_int *a, mp_int *b) MP_WUR;
mp_err s_mp_balance_mul(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;
mp_err s_mp_karatsuba_mul(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;
mp_err s_mp_toom_mul(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;
mp_err s_mp_karatsuba_sqr(const mp_int *a, mp_int *b) MP_WUR;
mp_err s_mp_toom_sqr(const mp_int *a, mp_int *b) MP_WUR;
mp_err s_mp_invmod_fast(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;
mp_err s_mp_invmod_slow(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;
mp_err s_mp_montgomery_reduce_fast(mp_int *x, const mp_int *n, mp_digit rho) MP_WUR;
mp_err s_mp_exptmod_fast(const mp_int *G, const mp_int *X, const mp_int *P, mp_int *Y, int redmode) MP_WUR;
mp_err s_mp_exptmod(const mp_int *G, const mp_int *X, const mp_int *P, mp_int *Y, int redmode) MP_WUR;
mp_err s_mp_rand_platform(void *p, size_t n) MP_WUR;
mp_err s_mp_prime_random_ex(mp_int *a, int t, int size, int flags, private_mp_prime_callback cb, void *dat);
void s_mp_reverse(unsigned char *s, int len);

/* TODO: jenkins prng is not thread safe as of now */
mp_err s_mp_rand_jenkins(void *p, size_t n) MP_WUR;
void s_mp_rand_jenkins_init(uint64_t seed);

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
mp_err func_name (mp_int * a, type b)                    \
{                                                        \
   int x = 0;                                            \
   mp_zero(a);                                           \
   while (b != 0u) {                                     \
      a->dp[x++] = ((mp_digit)b & MP_MASK);              \
      if (MP_SIZEOF_BITS(b) <= MP_DIGIT_BIT) { break; } \
      b >>= ((MP_SIZEOF_BITS(b) <= MP_DIGIT_BIT) ? 0 : MP_DIGIT_BIT); \
   }                                                     \
   a->used = x;                                          \
   return MP_OKAY;                                       \
}

/* deprecated functions */
MP_DEPRECATED(s_mp_invmod_fast) mp_err fast_mp_invmod(const mp_int *a, const mp_int *b, mp_int *c);
MP_DEPRECATED(s_mp_montgomery_reduce_fast) mp_err fast_mp_montgomery_reduce(mp_int *x, const mp_int *n, mp_digit rho);
MP_DEPRECATED(s_mp_mul_digs_fast) mp_err fast_s_mp_mul_digs(const mp_int *a, const mp_int *b, mp_int *c, int digs);
MP_DEPRECATED(s_mp_mul_high_digs_fast) mp_err fast_s_mp_mul_high_digs(const mp_int *a, const mp_int *b, mp_int *c,
      int digs);
MP_DEPRECATED(s_mp_sqr_fast) mp_err fast_s_mp_sqr(const mp_int *a, mp_int *b);
MP_DEPRECATED(s_mp_balance_mul) mp_err mp_balance_mul(const mp_int *a, const mp_int *b, mp_int *c);
MP_DEPRECATED(s_mp_exptmod_fast) mp_err mp_exptmod_fast(const mp_int *G, const mp_int *X, const mp_int *P, mp_int *Y,
      int redmode);
MP_DEPRECATED(s_mp_invmod_slow) mp_err mp_invmod_slow(const mp_int *a, const mp_int *b, mp_int *c);
MP_DEPRECATED(s_mp_karatsuba_mul) mp_err mp_karatsuba_mul(const mp_int *a, const mp_int *b, mp_int *c);
MP_DEPRECATED(s_mp_karatsuba_sqr) mp_err mp_karatsuba_sqr(const mp_int *a, mp_int *b);
MP_DEPRECATED(s_mp_toom_mul) mp_err mp_toom_mul(const mp_int *a, const mp_int *b, mp_int *c);
MP_DEPRECATED(s_mp_toom_sqr) mp_err mp_toom_sqr(const mp_int *a, mp_int *b);
MP_DEPRECATED(s_mp_reverse) void bn_reverse(unsigned char *s, int len);

#ifdef __cplusplus
}
#endif

#endif
