#include "tommath_private.h"
#ifdef BN_MP_MUL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* high level multiplication (handles sign) */
int mp_mul(const mp_int *a, const mp_int *b, mp_int *c)
{
   int res, neg, min_len, max_len, digs;
   min_len = MIN(a->used, b->used);
   max_len = MAX(a->used, b->used);
   digs = a->used + b->used + 1;
   neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;

   if (MP_HAS(MP_BALANCE_MUL) &&
       /* Check sizes. The smaller one needs to be larger than the Karatsuba cut-off.
        * The bigger one needs to be at least about one KARATSUBA_MUL_CUTOFF bigger
        * to make some sense, but it depends on architecture, OS, position of the
        * stars... so YMMV.
        * Using it to cut the input into slices small enough for fast_s_mp_mul_digs
        * was actually slower on the author's machine, but YMMV.
        */
       (min_len >= KARATSUBA_MUL_CUTOFF) &&
       (max_len / 2 >= KARATSUBA_MUL_CUTOFF) &&
       /* Not much effect was observed below a ratio of 1:2, but again: YMMV. */
       (max_len >= (2 * min_len))) {
      res = mp_balance_mul(a,b,c);
   } else if (MP_HAS(MP_TOOM_MUL) &&
              min_len >= TOOM_MUL_CUTOFF) {
      res = mp_toom_mul(a, b, c);
   } else if (MP_HAS(MP_KARATSUBA_MUL) &&
              min_len >= KARATSUBA_MUL_CUTOFF) {
      res = mp_karatsuba_mul(a, b, c);
   } else if (MP_HAS(FAST_S_MP_MUL_DIGS) &&
              /* can we use the fast multiplier?
               *
               * The fast multiplier can be used if the output will
               * have less than MP_WARRAY digits and the number of
               * digits won't affect carry propagation
               */
              (digs < (int)MP_WARRAY) &&
              (min_len <=
               (int)(1u << ((CHAR_BIT * sizeof(mp_word)) - (2u * (size_t)DIGIT_BIT))))) {
      res = fast_s_mp_mul_digs(a, b, c, digs);
   } else if (MP_HAS(S_MP_MUL_DIGS)) {
      res = s_mp_mul(a, b, c); /* uses s_mp_mul_digs */
   } else {
      res = MP_VAL;
   }
   c->sign = (c->used > 0) ? neg : MP_ZPOS;
   return res;
}
#endif
