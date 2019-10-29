#include "tommath_private.h"
#ifdef MP_MUL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* high level multiplication (handles sign) */
mp_err mp_mul(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_err err;
   int min = MP_MIN(a->used, b->used),
       max = MP_MAX(a->used, b->used),
       digs = a->used + b->used + 1;
   mp_sign neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;

   if (MP_HAS(S_MP_BALANCE_MUL) &&
       /* Check sizes. The smaller one needs to be larger than the Karatsuba cut-off.
        * The bigger one needs to be at least about one MP_KARATSUBA_MUL_CUTOFF bigger
        * to make some sense, but it depends on architecture, OS, position of the
        * stars... so YMMV.
        * Using it to cut the input into slices small enough for s_mp_mul_digs_fast
        * was actually slower on the author's machine, but YMMV.
        */
       (min >= MP_KARATSUBA_MUL_CUTOFF) &&
       ((max / 2) >= MP_KARATSUBA_MUL_CUTOFF) &&
       /* Not much effect was observed below a ratio of 1:2, but again: YMMV. */
       (max >= (2 * min))) {
      err = s_mp_balance_mul(a,b,c);
   } else if (MP_HAS(S_MP_TOOM_MUL) &&
              (min >= MP_TOOM_MUL_CUTOFF)) {
      err = s_mp_toom_mul(a, b, c);
   } else if (MP_HAS(S_MP_KARATSUBA_MUL) &&
              (min >= MP_KARATSUBA_MUL_CUTOFF)) {
      err = s_mp_karatsuba_mul(a, b, c);
   } else if (MP_HAS(S_MP_MUL_DIGS_FAST) &&
              /* can we use the fast multiplier?
               *
               * The fast multiplier can be used if the output will
               * have less than MP_WARRAY digits and the number of
               * digits won't affect carry propagation
               */
              (digs < MP_WARRAY) &&
              (min <= MP_MAXFAST)) {
      err = s_mp_mul_digs_fast(a, b, c, digs);
   } else if (MP_HAS(S_MP_MUL_DIGS)) {
      err = s_mp_mul_digs(a, b, c, digs);
   } else {
      err = MP_VAL;
   }
   c->sign = (c->used > 0) ? neg : MP_ZPOS;
   return err;
}
#endif
