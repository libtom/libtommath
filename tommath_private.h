/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifndef TOMMATH_PRIVATE_H_
#define TOMMATH_PRIVATE_H_

#include "tommath.h"
#include "tommath_class.h"
#include <limits.h>

/*
 * Private symbols
 * ---------------
 *
 * On Unix symbols can be marked as hidden if libtommath is compiled
 * as a shared object. By default, symbols are visible.
 * On Win32 a .def file must be used to specify the exported symbols.
 */
#if defined(__GNUC__) && __GNUC__ >= 4 && !defined(_WIN32) && !defined(__CYGWIN__)
#   define MP_PRIVATE __attribute__ ((visibility ("hidden")))
#else
#   define MP_PRIVATE
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
#  define MP_FREE_BUF(mem, size)   MP_FREE((mem), (size))
#  define MP_FREE_DIGS(mem, digits) MP_FREE((mem), sizeof (mp_digit) * (size_t)(digits))
#else
#  define MP_FREE_BUF(mem, size)                        \
do {                                                    \
   size_t fs_ = (size);                                 \
   void* fm_ = (mem);                                   \
   if (fm_ != NULL) {                                   \
      s_mp_zero_buf(fm_, fs_);                          \
      MP_FREE(fm_, fs_);                                \
   }                                                    \
} while (0)
#  define MP_FREE_DIGS(mem, digits)                     \
do {                                                    \
   int fd_ = (digits);                                  \
   mp_digit* fm_ = (mem);                               \
   if (fm_ != NULL) {                                   \
      s_mp_zero_digs(fm_, fd_);                         \
      MP_FREE(fm_, sizeof (mp_digit) * (size_t)fd_);    \
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
#  define MP_MUL_KARATSUBA_CUTOFF MP_DEFAULT_MUL_KARATSUBA_CUTOFF
#  define MP_SQR_KARATSUBA_CUTOFF MP_DEFAULT_SQR_KARATSUBA_CUTOFF
#  define MP_MUL_TOOM_CUTOFF      MP_DEFAULT_MUL_TOOM_CUTOFF
#  define MP_SQR_TOOM_CUTOFF      MP_DEFAULT_SQR_TOOM_CUTOFF
#  define MP_MUL_NTT_CUTOFF       MP_DEFAULT_MUL_NTT_CUTOFF
#  define MP_SQR_NTT_CUTOFF       MP_DEFAULT_SQR_NTT_CUTOFF
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

/* feature detection macro */
#ifdef _MSC_VER
/* Prevent false positive: not enough arguments for function-like macro invocation */
#pragma warning(disable: 4003)
#endif
#define MP_STRINGIZE(x)  MP__STRINGIZE(x)
#define MP__STRINGIZE(x) ""#x""
#define MP_HAS(x)        (sizeof(MP_STRINGIZE(x##_C)) == 1u)

#define MP_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MP_MAX(x, y) (((x) > (y)) ? (x) : (y))

#define MP_TOUPPER(c) ((((c) >= 'a') && ((c) <= 'z')) ? (((c) + 'A') - 'a') : (c))

#define MP_EXCH(t, a, b) do { t _c = a; a = b; b = _c; } while (0)

#define MP_IS_2EXPT(x) (((x) != 0u) && (((x) & ((x) - 1u)) == 0u))

/* TODO: same as above for bigint, merge (is it used elsewhere?) or change name */
#define MP_IS_POWER_OF_TWO(a)    (((mp_count_bits((a)) - 1) ==  mp_cnt_lsb((a))) )

/* Static assertion */
#define MP_STATIC_ASSERT(msg, cond) typedef char mp_static_assert_##msg[(cond) ? 1 : -1];

#define MP_SIZEOF_BITS(type)    ((size_t)CHAR_BIT * sizeof(type))

#define MP_MAX_COMBA            (int)(1uL << (MP_SIZEOF_BITS(mp_word) - (2u * (size_t)MP_DIGIT_BIT)))
#define MP_WARRAY               (int)(1uL << ((MP_SIZEOF_BITS(mp_word) - (2u * (size_t)MP_DIGIT_BIT)) + 1u))

#if defined(MP_16BIT)
typedef uint32_t mp_word;
#define MP_WORD_SIZE  4
#elif ((defined (MP_64BIT)) && !(defined(MP_31BIT)) )
typedef unsigned long mp_word __attribute__((mode(TI)));
#define MP_WORD_SIZE  16
#else
typedef uint64_t mp_word;
#define MP_WORD_SIZE  8
#endif

MP_STATIC_ASSERT(correct_word_size, sizeof(mp_word) == (2u * sizeof(mp_digit)))

/* default number of digits */
#ifndef MP_DEFAULT_DIGIT_COUNT
#   ifndef MP_LOW_MEM
#      define MP_DEFAULT_DIGIT_COUNT 32
#   else
#      define MP_DEFAULT_DIGIT_COUNT 8
#   endif
#endif

/* Minimum number of available digits in mp_int, MP_DEFAULT_DIGIT_COUNT >= MP_MIN_DIGIT_COUNT
 * - Must be at least 3 for s_mp_div_school.
 * - Must be large enough such that the mp_set_u64 setter can
 *   store uint64_t in the mp_int without growing
 */
#ifndef MP_MIN_DIGIT_COUNT
#    define MP_MIN_DIGIT_COUNT MP_MAX(3, (((int)MP_SIZEOF_BITS(uint64_t) + MP_DIGIT_BIT) - 1) / MP_DIGIT_BIT)
#endif
MP_STATIC_ASSERT(prec_geq_min_prec, MP_DEFAULT_DIGIT_COUNT >= MP_MIN_DIGIT_COUNT)
MP_STATIC_ASSERT(min_prec_geq_3, MP_MIN_DIGIT_COUNT >= 3)
MP_STATIC_ASSERT(min_prec_geq_uint64size,
                 MP_MIN_DIGIT_COUNT >= ((((int)MP_SIZEOF_BITS(uint64_t) + MP_DIGIT_BIT) - 1) / MP_DIGIT_BIT))

/* Maximum number of digits.
 * - Must be small enough such that mp_bit_count does not overflow.
 * - Must be small enough such that mp_radix_size for base 2 does not overflow.
 *   mp_radix_size needs two additional bytes for zero termination and sign.
 */
#define MP_MAX_DIGIT_COUNT ((INT_MAX - 2) / MP_DIGIT_BIT)

#if defined(__STDC_IEC_559__) || defined(__GCC_IEC_559) \
   || defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) \
   || defined(__i386__) || defined(_M_X86) || defined(_M_IX86) \
   || defined(__aarch64__) || defined(__arm__)
#define MP_HAS_SET_DOUBLE
#endif

/*
  The mp_log functions rely on the size of mp_word being larger than INT_MAX and in case
  there is a really weird architecture we try to check for it. Not a 100% reliable
  test but it has a safe fallback.
 */
#if !(((UINT_MAX == UINT32_MAX) && (MP_WORD_SIZE > 4)) \
      || ((UINT_MAX == UINT16_MAX) && (MP_WORD_SIZE > 2)))
#define S_MP_WORD_TOO_SMALL_C
#endif

/* random number source */
extern MP_PRIVATE mp_err(*s_mp_rand_source)(void *out, size_t size);

/* lowlevel functions, do not call! */
MP_PRIVATE bool s_mp_get_bit(const mp_int *a, int b) MP_WUR;
MP_PRIVATE int s_mp_log_2expt(const mp_int *a, mp_digit base) MP_WUR;

MP_PRIVATE mp_err s_mp_add(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;
MP_PRIVATE mp_err s_mp_div_3(const mp_int *a, mp_int *c, mp_digit *d) MP_WUR;
MP_PRIVATE mp_err s_mp_div_recursive(const mp_int *a, const mp_int *b, mp_int *q, mp_int *r) MP_WUR;
MP_PRIVATE mp_err s_mp_div_school(const mp_int *a, const mp_int *b, mp_int *c, mp_int *d) MP_WUR;
MP_PRIVATE mp_err s_mp_div_small(const mp_int *a, const mp_int *b, mp_int *c, mp_int *d) MP_WUR;
MP_PRIVATE mp_err s_mp_exptmod(const mp_int *G, const mp_int *X, const mp_int *P, mp_int *Y, int redmode) MP_WUR;
MP_PRIVATE mp_err s_mp_exptmod_fast(const mp_int *G, const mp_int *X, const mp_int *P, mp_int *Y, int redmode) MP_WUR;
MP_PRIVATE mp_err s_mp_invmod(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;
MP_PRIVATE mp_err s_mp_invmod_odd(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;

MP_PRIVATE mp_err s_mp_montgomery_reduce_comba(mp_int *x, const mp_int *n, mp_digit rho) MP_WUR;
MP_PRIVATE mp_err s_mp_mul(const mp_int *a, const mp_int *b, mp_int *c, int digs) MP_WUR;
MP_PRIVATE mp_err s_mp_mul_balance(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;
MP_PRIVATE mp_err s_mp_mul_comba(const mp_int *a, const mp_int *b, mp_int *c, int digs) MP_WUR;
MP_PRIVATE mp_err s_mp_mul_high(const mp_int *a, const mp_int *b, mp_int *c, int digs) MP_WUR;
MP_PRIVATE mp_err s_mp_mul_high_comba(const mp_int *a, const mp_int *b, mp_int *c, int digs) MP_WUR;
MP_PRIVATE mp_err s_mp_mul_karatsuba(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;
MP_PRIVATE mp_err s_mp_mul_toom(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;

/* NTT stuff, it's a lot */

/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)

#include <limits.h>

/* Magic numbers */
#define NTT_BASE_SIZE         (sizeof(mp_digit) * CHAR_BIT)
#define NTT_BASE_SIZE_M_ONE   (NTT_BASE_SIZE - 1)
#define NTT_LARGE_SIZE        (2 * NTT_BASE_SIZE)
#define NTT_LIMB_SIZE         MP_DIGIT_BIT
#define NTT_LIMB_SIZE_M_ONE   (NTT_LIMB_SIZE - 1)

#if (NTT_LIMB_SIZE == 60)
#define NTT_PRIME_MAX_SIZE    (NTT_BASE_SIZE - 2)
#else
#define NTT_PRIME_MAX_SIZE    (NTT_BASE_SIZE - 2)
#endif

/* Indirectly influences the number of primes */
#define NTT_BOUND_BIT_MULTIPLICATOR  2


/* One coeff holds 2 limbs at most (120 bits for MP_64BIT)  */
#if (NTT_LIMB_SIZE == 60)
#define NTT_QUARTER_COEFF     (NTT_LIMB_SIZE/2)
#else
#define NTT_QUARTER_COEFF     (NTT_LIMB_SIZE)
#endif

/* TODO: make tunable, although the tuning runs would take a a lot of time */
/*
  Weights for radix-n when computing optimal coefficient-array length.
  Assuming radix 2 is the fastest, radix 5 the slowest.
  Profiling would be a better idea than the general values below.
 */
#define NTT_SMOOTH_W2   1
#define NTT_SMOOTH_W3   2
#define NTT_SMOOTH_W5   6

#define NTT_MR_MAXSTAGE 48   /* stages of m: log2(m) plus the 3s and 5s */

/* Three primes for 60 bits, four for 28 bit */
#define NTT_MAXP 6           /* max CRT primes */

#if (NTT_LIMB_SIZE == 60)
#define NTT_MAXW 4            /* max limbs per coefficient */
#else
#define NTT_MAXW 6
#endif

/* TODO: Basically MP_MASK. Use directly? */
#define NTT_MASK (((mp_digit)1 << NTT_LIMB_SIZE) - 1)

/* Montgomery context for a prime p < 2^62, R = 2^64. */
typedef struct {
   mp_digit p;      /* The prime p */
   mp_digit ninv;   /* (-p^{-1}) mod R  */
   mp_digit r2;     /* R^2 mod p */
} ntt_mont;

typedef struct {
   mp_digit  p;      /* The prime p */
   mp_digit  root;   /* primitive 2^s-th (or m-th) root mod p, ordinary residue */
   int  s;           /* root order is a power of two */
   int  m;           /* 5-smooth transform length of the root order */
} ntt_prime_ent;

/* Root table and butterfly constants of a single smooth transform. */
typedef struct {
   int  m;
   int  nstages;
   int  radix[NTT_MR_MAXSTAGE];
   int  ell[NTT_MR_MAXSTAGE];        /* block length at each stage */
   mp_digit *w;                      /* w[s] = zeta^s, Montgomery form, s < m */
   mp_digit  c3, c3i;                /* primitive cube root, and its inverse */
   mp_digit  k5[4], k5i[4];          /* radix-5 constants, forward / inverse */
   bool has3;
   bool has5;
} ntt_sroots;

typedef struct {
   int           t;                        /* number of primes */
   int           width;                    /* 60-bit limbs per coefficient */
   mp_digit      p[NTT_MAXP];              /* the primes, ascending */
   ntt_mont      mc[NTT_MAXP];             /* Montgomery context of each prime */
   mp_digit      minv[NTT_MAXP][NTT_MAXP]; /* p_i^{-1} * 2^64 mod p_j, i < j */
   mp_digit      x[NTT_MAXP];              /* scratch */
   mp_digit      d[NTT_MAXP];              /* mixed-radix digits */
} ntt_crt_ctx;

/* The main function, the rest are helpers */
MP_PRIVATE mp_err s_mp_mul_ntt(const mp_int *a, const mp_int *b, mp_int *c);
MP_PRIVATE mp_err s_mp_sqr_ntt(const mp_int *a, mp_int *c);
/* ceil(log_2(x)) */
MP_PRIVATE int s_mp_ntt_ilog2_ceil(int x);
/* Convolution of a single radix */
MP_PRIVATE mp_err s_mp_ntt_convolve_single(const mp_int *a, const mp_int *b, const ntt_prime_ent *ent, mp_digit *ra,
      mp_digit *rb, mp_digit *out, int ncoef);
MP_PRIVATE mp_err s_mp_ntt_autoconvolve_single(const mp_int *a, const ntt_prime_ent *ent, mp_digit *ra,
      mp_digit *out, int ncoef);

/* Forward butterflies */
MP_PRIVATE void s_mp_ntt_fwd2(const ntt_mont *mc, mp_digit *b, int L, int step, const ntt_sroots *R);
MP_PRIVATE void s_mp_ntt_fwd3(const ntt_mont *mc, mp_digit *b, int L, int step, const ntt_sroots *R);
MP_PRIVATE void s_mp_ntt_fwd5(const ntt_mont *mc, mp_digit *b, int L, int step, const ntt_sroots *R);

/* Inverse butterflies */
MP_PRIVATE void s_mp_ntt_inv2(const ntt_mont *mc, mp_digit *b, int L, int step, const ntt_sroots *R);
MP_PRIVATE void s_mp_ntt_inv3(const ntt_mont *mc, mp_digit *b, int L, int step, const ntt_sroots *R);
MP_PRIVATE void s_mp_ntt_inv5(const ntt_mont *mc, mp_digit *b, int L, int step, const ntt_sroots *R);

/* Chinese remainder to get teh differrent radices together */
MP_PRIVATE void s_mp_ntt_crt_init(ntt_crt_ctx *cx, const mp_digit *primes, int t, int bound_bits);
MP_PRIVATE void s_mp_ntt_crt_coeff(ntt_crt_ctx *cx, const mp_digit *r, mp_digit *limbs);
MP_PRIVATE mp_err s_mp_ntt_crt_assemble(ntt_crt_ctx *cx, mp_digit **residues, int nprimes,int ncoef, mp_int *c);

/* Single mp_digit modular inverse. Was here but got out, div_exact* uses it, too */
/* Initialize ntt_mont structure with prime p */
MP_PRIVATE void s_mp_ntt_mont_init(ntt_mont *m, mp_digit p);
/* Montgomery reduction */
MP_PRIVATE mp_digit s_mp_ntt_mont_redc(const ntt_mont *m, mp_word T);
/* Montgomery to plain */
MP_PRIVATE mp_digit s_mp_ntt_from_mont(const ntt_mont *m, mp_digit a);
/* Plain to Montgomery */
MP_PRIVATE mp_digit s_mp_ntt_to_mont(const ntt_mont *m, mp_digit x);

/* Montgomery arithmetic */
MP_PRIVATE mp_digit s_mp_ntt_mont_add(const ntt_mont *m, mp_digit a, mp_digit b);
MP_PRIVATE mp_digit s_mp_ntt_mont_mul(const ntt_mont *m, mp_digit a, mp_digit b);
MP_PRIVATE mp_digit s_mp_ntt_mont_pow(const ntt_mont *m, mp_digit base, mp_digit exp);
MP_PRIVATE mp_digit s_mp_ntt_mont_sub(const ntt_mont *m, mp_digit a, mp_digit b);
/* Plain arithmetic. Helpers for bootstrapping the primes */
MP_PRIVATE mp_digit s_mp_ntt_small_mulmod(mp_digit a, mp_digit b, mp_digit m);
MP_PRIVATE mp_digit s_mp_ntt_small_powmod(mp_digit b, mp_digit e, mp_digit m);

/* Selects forward butterfly according to radix */
MP_PRIVATE void s_mp_ntt_smooth_fwd(ntt_mont *mc, mp_digit *a, int m, const ntt_sroots *R);
/* Selects forward butterfly according to radix */
MP_PRIVATE void s_mp_ntt_smooth_inv(ntt_mont *mc, mp_digit *a, int m, const ntt_sroots *R);

#ifdef USE_NAIVE
/* Choose length of the coefficient array */
/* All weights 1 */
MP_PRIVATE int s_mp_ntt_smooth_length_naive(int n_min);
#else
/* Takes different weights per radix*/
MP_PRIVATE int s_mp_ntt_smooth_length_weighted(int n_min, int w2, int w3, int w5);
#endif
/* Choose length of the coefficient array, given minimum. Returns n >= need */
MP_PRIVATE int s_mp_ntt_smooth_length(int need);
/* Organizes the number of radix calls */
MP_PRIVATE int s_mp_ntt_smooth_plan(int m, int *radix, int max);

/* Memory for root array */
MP_PRIVATE mp_err s_mp_ntt_sroots_init(ntt_sroots *R, ntt_mont *mc, int m, mp_digit zeta);
MP_PRIVATE void s_mp_ntt_sroots_free(ntt_sroots *R);

/* Gnerate prime for NTT round */
MP_PRIVATE bool s_mp_ntt_find_smooth_prime(ntt_prime_ent *ent, int m, int index, mp_digit min_p);
/* Small number primetest */
MP_PRIVATE bool s_mp_ntt_small_is_prime(mp_digit n);
/* The NTT primes are expected in a certain order */
MP_PRIVATE void s_mp_ntt_sort_primes(ntt_prime_ent *ents, int n);

#endif
/* NTT stuff ends here. Yes, it was a lot, as promised.  */



MP_PRIVATE mp_err s_mp_prime_is_divisible(const mp_int *a, bool *result) MP_WUR;
MP_PRIVATE mp_err s_mp_rand_platform(void *p, size_t n) MP_WUR;
MP_PRIVATE mp_err s_mp_sqr(const mp_int *a, mp_int *b) MP_WUR;
MP_PRIVATE mp_err s_mp_sqr_comba(const mp_int *a, mp_int *b) MP_WUR;
MP_PRIVATE mp_err s_mp_sqr_karatsuba(const mp_int *a, mp_int *b) MP_WUR;
MP_PRIVATE mp_err s_mp_sqr_toom(const mp_int *a, mp_int *b) MP_WUR;
MP_PRIVATE mp_err s_mp_sub(const mp_int *a, const mp_int *b, mp_int *c) MP_WUR;
MP_PRIVATE void s_mp_copy_digs(mp_digit *d, const mp_digit *s, int digits);
MP_PRIVATE void s_mp_zero_buf(void *mem, size_t size);
MP_PRIVATE void s_mp_zero_digs(mp_digit *d, int digits);
MP_PRIVATE mp_err s_mp_radix_size_overestimate(const mp_int *a, const int radix, size_t *size);

#define MP_PRECISION_FIXED_LOG   ( (int) (((sizeof(mp_word) * CHAR_BIT) / 2) - 1))
#define MP_UPPER_LIMIT_FIXED_LOG ( (int) ( (sizeof(mp_word) * CHAR_BIT) - 1))

/* Compute the modular inverse b^(-1) mod 2^MP_DIGIT_BIT */
MP_PRIVATE mp_digit s_mp_ntt_invmod_d(mp_digit p);

MP_PRIVATE mp_err s_mp_fp_log(const mp_int *a, mp_int *c) MP_WUR;
MP_PRIVATE mp_err s_mp_fp_log_d(const mp_int *a, mp_word *c) MP_WUR;

#ifdef MP_SMALL_STACK_SIZE

#if defined(__GNUC__)
/* We use TLS (Thread Local Storage) to manage the instance of the WARRAY
 * per thread.
 * The compilers we're usually looking at are GCC, Clang and MSVC.
 * Both GCC and Clang are straight-forward with TLS, so it's enabled there.
 * Using MSVC the tests were OK with the static library, but failed when
 * the library was built as a DLL. As a result we completely disable
 * support for MSVC.
 * If your compiler can handle TLS properly without too much hocus pocus,
 * feel free to open a PR to add support for it.
 */
#define mp_thread __thread
#else
#error "MP_SMALL_STACK_SIZE not supported with your compiler"
#endif

#define MP_SMALL_STACK_SIZE_C
#define MP_ALLOC_WARRAY(name) *name = s_mp_warray_get()
#define MP_FREE_WARRAY(name) s_mp_warray_put(name)
#define MP_CHECK_WARRAY(name) do { if ((name) == NULL) { return MP_MEM; } } while(0)
#else
#define MP_ALLOC_WARRAY(name) name[MP_WARRAY]
#define MP_FREE_WARRAY(name)
#define MP_CHECK_WARRAY(name)
#endif

#ifndef mp_thread
#define mp_thread
#endif

typedef struct {
   void *w_free, *w_used;
} st_warray;

extern MP_PRIVATE mp_thread st_warray s_mp_warray;

MP_PRIVATE void *s_mp_warray_get(void);
MP_PRIVATE void s_mp_warray_put(void *w);

#define MP_RADIX_MAP_REVERSE_SIZE 80u
extern MP_PRIVATE const char s_mp_radix_map[];
extern MP_PRIVATE const uint8_t s_mp_radix_map_reverse[];
extern MP_PRIVATE const mp_digit s_mp_prime_tab[];

/* number of primes */
#define MP_PRIME_TAB_SIZE 256

#define MP_GET_ENDIANNESS(x) \
   do{\
      int16_t n = 0x1;                                          \
      char *p = (char *)&n;                                     \
      x = (p[0] == '\x01') ? MP_LITTLE_ENDIAN : MP_BIG_ENDIAN;  \
   } while (0)

/* code-generating macros */
#define MP_SET_UNSIGNED(name, type)                                                    \
    void name(mp_int * a, type b)                                                      \
    {                                                                                  \
        int i = 0;                                                                     \
        while (b != 0u) {                                                              \
            a->dp[i++] = ((mp_digit)b & MP_MASK);                                      \
            if (MP_SIZEOF_BITS(type) <= MP_DIGIT_BIT) { break; }                       \
            b >>= ((MP_SIZEOF_BITS(type) <= MP_DIGIT_BIT) ? 0 : MP_DIGIT_BIT);         \
        }                                                                              \
        a->used = i;                                                                   \
        a->sign = MP_ZPOS;                                                             \
        s_mp_zero_digs(a->dp + a->used, a->alloc - a->used);                         \
    }

#define MP_SET_SIGNED(name, uname, type, utype)          \
    void name(mp_int * a, type b)                        \
    {                                                    \
        uname(a, (b < 0) ? -(utype)b : (utype)b);        \
        if (b < 0) { a->sign = MP_NEG; }                 \
    }

#define MP_INIT_INT(name , set, type)                    \
    mp_err name(mp_int * a, type b)                      \
    {                                                    \
        mp_err err;                                      \
        if ((err = mp_init(a)) != MP_OKAY) {             \
            return err;                                  \
        }                                                \
        set(a, b);                                       \
        return MP_OKAY;                                  \
    }

#define MP_GET_MAG(name, type)                                                         \
    type name(const mp_int* a)                                                         \
    {                                                                                  \
        int i = MP_MIN(a->used, (int)((MP_SIZEOF_BITS(type) + MP_DIGIT_BIT - 1) / MP_DIGIT_BIT)); \
        type res = 0u;                                                                 \
        while (i --> 0) {                                                              \
            res <<= ((MP_SIZEOF_BITS(type) <= MP_DIGIT_BIT) ? 0 : MP_DIGIT_BIT);       \
            res |= (type)a->dp[i];                                                     \
            if (MP_SIZEOF_BITS(type) <= MP_DIGIT_BIT) { break; }                       \
        }                                                                              \
        return res;                                                                    \
    }

#define MP_GET_SIGNED(name, mag, type, utype)                 \
    type name(const mp_int* a)                                \
    {                                                         \
        utype res = mag(a);                                   \
        return mp_isneg(a) ? (type)-res : (type)res;          \
    }

#endif
