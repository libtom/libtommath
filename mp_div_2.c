#include "tommath_private.h"
#ifdef MP_DIV_2_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* b = a/2 */
mp_err mp_div_2(const mp_int *a, mp_int *b)
{
   size_t x, oldused;
   mp_digit r;

   if (b->alloc < a->used) {
      mp_err err;
      if ((err = mp_grow(b, a->used)) != MP_OKAY) {
         return err;
      }
   }

   oldused = b->used;
   b->used = a->used;

   /* carry */
   r = 0;
   for (x = b->used; x --> 0;) {
      /* get the carry for the next iteration */
      mp_digit rr = a->dp[x] & 1u;

      /* shift the current digit, add in carry and store */
      b->dp[x] = (a->dp[x] >> 1) | (r << (MP_DIGIT_BIT - 1));

      /* forward carry to next iteration */
      r = rr;
   }

   /* zero excess digits */
   MP_ZERO_DIGITS_NEW(b->dp + b->used, b->dp + oldused);

   b->sign = a->sign;
   mp_clamp(b);
   return MP_OKAY;
}
#endif
