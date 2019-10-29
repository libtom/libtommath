#include "tommath_private.h"
#ifdef MP_MUL_D_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* multiply by a digit */
mp_err mp_mul_d(const mp_int *a, mp_digit b, mp_int *c)
{
   mp_digit u;
   mp_err   err;
   int   ix, oldused;

   /* make sure c is big enough to hold a*b */
   if (c->alloc < (a->used + 1)) {
      if ((err = mp_grow(c, a->used + 1)) != MP_OKAY) {
         return err;
      }
   }

   /* get the original destinations used count */
   oldused = c->used;

   /* set the sign */
   c->sign = a->sign;

   /* zero carry */
   u = 0;

   /* compute columns */
   for (ix = 0; ix < a->used; ix++) {
      /* compute product and carry sum for this term */
      mp_word r       = (mp_word)u + ((mp_word)a->dp[ix] * (mp_word)b);

      /* mask off higher bits to get a single digit */
      c->dp[ix] = (mp_digit)(r & (mp_word)MP_MASK);

      /* send carry into next iteration */
      u       = (mp_digit)(r >> (mp_word)MP_DIGIT_BIT);
   }

   /* store final carry [if any] and increment ix offset  */
   c->dp[ix] = u;

   /* set used count */
   c->used = a->used + 1;

   /* now zero digits above the top */
   MP_ZERO_DIGITS(c->dp + c->used, oldused - c->used);

   mp_clamp(c);

   return MP_OKAY;
}
#endif
