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
        * The bigger one needs to be at least about one MP_MUL_KARATSUBA_CUTOFF bigger
        * to make some sense, but it depends on architecture, OS, position of the
        * stars... so YMMV.
        * Using it to cut the input into slices small enough for s_mp_mul_comba
        * was actually slower on the author's machine, but YMMV.
        */
       (min >= MP_MUL_KARATSUBA_CUTOFF) &&
       ((max / 2) >= MP_MUL_KARATSUBA_CUTOFF) &&
       /* Not much effect was observed below a ratio of 1:2, but again: YMMV. */
       (max >= (2 * min))) {
      err = s_mp_mul_balance(a,b,c);
   } else if (MP_HAS(S_MP_MUL_TOOM) &&
              (min >= MP_MUL_TOOM_CUTOFF)) {
      err = s_mp_mul_toom(a, b, c);
   } else if (MP_HAS(S_MP_MUL_KARATSUBA) &&
              (min >= MP_MUL_KARATSUBA_CUTOFF)) {
      err = s_mp_mul_karatsuba(a, b, c);
   } else if (MP_HAS(S_MP_MUL_COMBA) &&
              (a->dp != c->dp) && (b->dp != c->dp) &&
              /* can we use the fast multiplier?
               *
               * The fast multiplier can be used if the number of
               * digits won't affect carry propagation
               */
              (min <= MP_MAX_COMBA)) {
      err = s_mp_mul_comba(a, b, c, digs);
   } else if (MP_HAS(S_MP_MUL)) {
      err = s_mp_mul(a, b, c, digs);
   } else {
      err = MP_VAL;
   }
   c->sign = (c->used > 0) ? neg : MP_ZPOS;
   return err;
}
#endif
