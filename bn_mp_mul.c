#include <tommath.h>
#ifdef BN_MP_MUL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */
#ifdef BN_MP_BALANCE_MUL
#   define MP_BMC1a (KARATSUBA_MUL_CUTOFF)
#   define MP_BMC1b (MP_BMC1a)
#   define MP_BMC2 0.001f
#   define MP_BMC3 0.999f
#endif
#ifdef BN_MP_FFT_MUL_C
/* Quite rough, assuming a lot */
#if _WIN32 || _WIN64
#   if _WIN64
#      define _ITS_64_BIT_
#   else
#      define _ITS_32_BIT_
#   endif
#elif __GNUC__
#   if __x86_64__ || __ppc64__
#      define _ITS_64_BIT_
#   else
#      define _ITS_32_BIT_
#   endif
#endif

#include <stdint.h>
#if (defined _ITS_64_BIT_) && (!(defined __LP64__))
static const int tab64[64] = {
  63, 0, 58, 1, 59, 47, 53, 2,
  60, 39, 48, 27, 54, 33, 42, 3,
  61, 51, 37, 40, 49, 18, 28, 20,
  55, 30, 34, 11, 43, 14, 22, 4,
  62, 57, 46, 52, 38, 26, 32, 41,
  50, 36, 17, 19, 29, 10, 13, 21,
  56, 45, 25, 31, 35, 16, 9, 12,
  44, 24, 15, 8, 23, 7, 6, 5
};
static int ilog2(uint64_t value)
{
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  value |= value >> 32;
  return tab64[((uint64_t) ((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >>
	       58];
}
#else
static const int tab32[32] = {
  0, 9, 1, 10, 13, 21, 2, 29,
  11, 14, 16, 18, 22, 25, 3, 30,
  8, 12, 20, 28, 15, 17, 24, 7,
  19, 27, 23, 6, 26, 5, 4, 31
};
static int ilog2(uint32_t value)
{
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  return tab32[(uint32_t) (value * 0x07C4ACDD) >> 27];
}
#endif

static const int powtwos[] = {
  1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096,
  8192, 16384, 32768, 65536, 131072, 262144, 524288,
  1048576, 2097152, 4194304, 8388608, 16777216, 33554432,
  67108864, 134217728, 268435456, 536870912, 1073741824
};
#endif

/* high level multiplication (handles sign) */
int mp_mul (mp_int * a, mp_int * b, mp_int * c)
{
  int     res, neg;
#ifdef BN_MP_BALANCE_MUL
  int len_a,len_b;
#endif
#ifdef BN_MP_FFT_MUL_C
int il2,extra;
#endif

  neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;

#ifdef BN_MP_BALANCE_MUL
  len_a = a->used;
  len_b = b->used;
  if(len_a == len_b)[
    goto GO_ON;
  }
  // FFT has its own
  if(MIN(len_a, len_b) >= FFT_MUL_CUTOFF){
   goto GO_ON;
  }
  /*
    Check sizes. The smaller one needs to be larger than the Karatsuba cut-off,
    the bigger one needs some minimum, too, as empiricals tests suggest.
   */
  if (MIN(len_a, len_b) < MP_BMC1a || MAX(len_a, len_b) < MP_BMC1b) {
    goto GO_ON;
  }
  /* check if the sizes of both differ enough in relation*/
  /* The first test is probably not necessary, may be even counter-productive */
  if ( ( ((float) MIN(len_a, len_b) / (float) MAX(len_a, len_b)) < MP_BMC2
      || ((float) MIN(len_a, len_b) / (float) MAX(len_a, len_b)) > MP_BMC3 )
      ) {
    goto GO_ON;
  }
  /* Make sure that a is the larger one but don't forget it */
  if (len_a < len_b) {
    mp_exch(a, b);
  }
  /* 
   * Cut larger one in two parts a1, a0 with the smaller part a0 of the same
   * length as the smaller input number b_0. Work on copy to make things simpler
   */
  if ((e = mp_init_size(&a_0, b->used + 1)) != MP_OKAY) {
    return e;
  }
  a_0.used = b->used;
  if ((e = mp_init_size(&a_1, a->used - b->used + 1)) != MP_OKAY) {
    return e;
  }
  a_1.used = a->used - b->used;
  /* fill smaller part a_0 */
  for (count = 0; count < b->used; count++) {
    a_0.dp[count] = a->dp[count];
  }
  /* fill bigger part a_1 with the counter already at the right place */
  for (; count < a->used; count++) {
    a_1.dp[count - b->used] = a->dp[count];
  }
  /* a_1 = a_1 * b_0 */
  /* a_1 and b_0 are of different size and might allow for another round */
  if ((e = mp_mul(&a_1, b, &a_1)) != MP_OKAY) {
    return e;
  }
  /* a_1 = a_1 * 2^(length(b_0)) */
  if ((e = mp_lshd(&a_1, b->used)) != MP_OKAY) {
    return e;
  }
  /* a_0 = a_0 * b_0 */
  if ((e = mp_mul(&a_0, b, &a_0)) != MP_OKAY) {
    return e;
  }
  /* c = a_1 + a_0 */
  if ((e = mp_add(&a_1, &a_0, c)) != MP_OKAY) {
    return e;
  }
  /* Don't mess with the input more than necessary */
  if (len_a < len_b) {
    mp_exch(a, b);
  }
  mp_clear_multi(&a_0, &a_1, NULL);

GO_ON:
#endif

#ifdef  MP_28BIT
  /* use FFT? */
#ifdef BN_MP_FFT_MUL_C
  /* 
     FFT has an upper limit caused by rounding erors. After one round of
     Toom-Cook it can cut in again.
   */
  if (MIN (a->used, b->used) >= FFT_MUL_CUTOFF &&
      MAX (a->used, b->used) <= FFT_UPPER_LIMIT ) {
    /* rounds the edges of the stair a bit (good for equal sized multipliers) */
    il2 = ilog2((unsigned int)MAX(a->used,b->used));
    extra = (il2<12)?powtwos[il2-2]:powtwos[8];
    if(MAX (a->used, b->used)  < powtwos[il2]+extra ){
      goto town;;
    }  
    res = mp_fft_mul(a, b, c);
  } else 
town:
#endif
#endif

#ifdef BN_MP_TOOM_COOK_5_MUL_C
  if (MIN (a->used, b->used) >= TOOM_COOK_5_MUL_CO) {
    res = mp_toom_cook_5_mul(a, b, c);
  } else 
#endif

#ifdef BN_MP_TOOM_COOK_4_MUL_C
  if (MIN (a->used, b->used) >= TOOM_COOK_4_MUL_CO) {
    res = mp_toom_cook_4_mul(a, b, c);
  } else 
#endif
  /* use Toom-Cook? */
#ifdef BN_MP_TOOM_MUL_C
  if (MIN (a->used, b->used) >= TOOM_MUL_CUTOFF) {
    res = mp_toom_mul(a, b, c);
  } else 
#endif
#ifdef BN_MP_KARATSUBA_MUL_C
  /* use Karatsuba? */
  if (MIN (a->used, b->used) >= KARATSUBA_MUL_CUTOFF) {
    res = mp_karatsuba_mul (a, b, c);
  } else 
#endif
  {
    /* can we use the fast multiplier?
     *
     * The fast multiplier can be used if the output will 
     * have less than MP_WARRAY digits and the number of 
     * digits won't affect carry propagation
     */
    int     digs = a->used + b->used + 1;

#ifdef BN_FAST_S_MP_MUL_DIGS_C
    if ((digs < MP_WARRAY) &&
        MIN(a->used, b->used) <= 
        (1 << ((CHAR_BIT * sizeof (mp_word)) - (2 * DIGIT_BIT)))) {
      res = fast_s_mp_mul_digs (a, b, c, digs);
    } else 
#endif
#ifdef BN_S_MP_MUL_DIGS_C
      res = s_mp_mul (a, b, c); /* uses s_mp_mul_digs */
#else
      res = MP_VAL;
#endif

  }
  c->sign = (c->used > 0) ? neg : MP_ZPOS;
  return res;
}
#endif

/* $Source$ */
/* $Revision: 0.41 $ */
/* $Date: 2007-04-18 09:58:18 +0000 $ */
