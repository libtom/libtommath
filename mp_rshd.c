#include "tommath_private.h"
#ifdef MP_RSHD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* shift right a certain amount of digits */
void mp_rshd(mp_int *a, size_t b)
{
   size_t x;

   /* if b > used then simply zero it and return */
   if (a->used <= b) {
      mp_zero(a);
      return;
   }

   /* shift the digits down.
    * this is implemented as a sliding window where
    * the window is b-digits long and digits from
    * the top of the window are copied to the bottom
    *
    * e.g.

    b-2 | b-1 | b0 | b1 | b2 | ... | bb |   ---->
                /\                   |      ---->
                 \-------------------/      ---->
    */
   for (x = 0; x < (a->used - b); x++) {
      a->dp[x] = a->dp[x + b];
   }

   /* zero the top digits */
   MP_ZERO_DIGITS_NEW(a->dp + a->used - b, a->dp + a->used);

   /* remove excess digits */
   a->used -= b;
}
#endif
