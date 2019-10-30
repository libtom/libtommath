#include "tommath_private.h"
#ifdef S_MP_SQR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* low level squaring, b = a*a, HAC pp.596-597, Algorithm 14.16 */
mp_err s_mp_sqr(const mp_int *a, mp_int *b)
{
   int ix, pa = a->used;

   for (ix = 0; ix < pa; ix++) {
      mp_digit u;
      int iy;

      /* first calculate the digit at 2*ix */
      /* calculate double precision result */
      mp_word r = (mp_word)b->dp[2*ix] +
                  ((mp_word)a->dp[ix] * (mp_word)a->dp[ix]);

      /* store lower part in result */
      b->dp[ix+ix] = (mp_digit)(r & (mp_word)MP_MASK);

      /* get the carry */
      u           = (mp_digit)(r >> (mp_word)MP_DIGIT_BIT);

      for (iy = ix + 1; iy < pa; iy++) {
         /* first calculate the product */
         r       = (mp_word)a->dp[ix] * (mp_word)a->dp[iy];

         /* now calculate the double precision result, note we use
          * addition instead of *2 since it's easier to optimize
          */
         r       = (mp_word)b->dp[ix + iy] + r + r + (mp_word)u;

         /* store lower part */
         b->dp[ix + iy] = (mp_digit)(r & (mp_word)MP_MASK);

         /* get carry */
         u       = (mp_digit)(r >> (mp_word)MP_DIGIT_BIT);
      }
      /* propagate upwards */
      while (u != 0uL) {
         r       = (mp_word)b->dp[ix + iy] + (mp_word)u;
         b->dp[ix + iy] = (mp_digit)(r & (mp_word)MP_MASK);
         u       = (mp_digit)(r >> (mp_word)MP_DIGIT_BIT);
         ++iy;
      }
   }

   b->used = (2 * pa) + 1;
   mp_clamp(b);
   return MP_OKAY;
}
#endif
