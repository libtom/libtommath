#include "tommath_private.h"
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
 * SPDX-License-Identifier: Unlicense
 */

/* high level multiplication (handles sign) */
int mp_mul(const mp_int *a, const mp_int *b, mp_int *c)
{
   int     res, neg;
#if ( ((defined LTM_USE_FASTER_VERSIONS) && (defined BN_MP_BALANCE_MUL_C)) && (defined BN_MP_KARATSUBA_MUL_C) )
   int len_b, len_a;
#endif
   neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;
   /*
      TODO: Assumes that nobody uses Toom-Cook-3 without Toom-Cook-2 (Karatsuba)
   */
#if ( ((defined LTM_USE_FASTER_VERSIONS) && (defined BN_MP_BALANCE_MUL_C)) && (defined BN_MP_KARATSUBA_MUL_C) )
   len_a = a->used;
   len_b = b->used;

   if (len_a == len_b) {
      goto LBL_GO_ON;
   }

   /*
    * Check sizes. The smaller one needs to be larger than the Karatsuba cut-off.
    * The bigger one needs to be at least about one KARATSUBA_MUL_CUTOFF bigger to make some
    * sense, but it depends on architecture, OS and position of the planets, so YMMV.
    */
   if (MIN(len_a, len_b) < KARATSUBA_MUL_CUTOFF
       || ((MAX(len_a, len_b)) / 2) < KARATSUBA_MUL_CUTOFF) {
      goto LBL_GO_ON;
   }
   /*
    * Not much effect was observed below a ratio of 1:2, but YMMV.
    */
   if (MAX(len_a, len_b) /  MIN(len_a, len_b) < 2) {
      goto LBL_GO_ON;
   }

   res = mp_balance_mul(a,b,c);
   goto LBL_END;

LBL_GO_ON:
#endif

   /* use Toom-Cook? */
#ifdef BN_MP_TOOM_MUL_C
   if (MIN(a->used, b->used) >= TOOM_MUL_CUTOFF) {
      res = mp_toom_mul(a, b, c);
   } else
#endif
#ifdef BN_MP_KARATSUBA_MUL_C
      /* use Karatsuba? */
      if (MIN(a->used, b->used) >= KARATSUBA_MUL_CUTOFF) {
         res = mp_karatsuba_mul(a, b, c);
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
         if ((digs < (int)MP_WARRAY) &&
             (MIN(a->used, b->used) <=
              (int)(1u << (((size_t)CHAR_BIT * sizeof(mp_word)) - (2u * (size_t)DIGIT_BIT))))) {
            res = fast_s_mp_mul_digs(a, b, c, digs);
         } else
#endif
         {
#ifdef BN_S_MP_MUL_DIGS_C
            res = s_mp_mul(a, b, c); /* uses s_mp_mul_digs */
#else
            res = MP_VAL;
#endif
         }
      }
#if ( ((defined LTM_USE_FASTER_VERSIONS) && (defined BN_MP_BALANCE_MUL_C)) && (defined BN_MP_KARATSUBA_MUL_C) )
LBL_END:
#endif
   c->sign = (c->used > 0) ? neg : MP_ZPOS;
   return res;
}
#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
