#include "tommath_private.h"
#ifdef S_MP_SQR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* low level squaring, b = a*a, HAC pp.596-597, Algorithm 14.16 */
mp_err s_mp_sqr(const mp_int *a, mp_int *b)
{
   mp_int   t;
   int      ix, pa;
   mp_err   err;

   pa = a->used;
   if ((err = mp_init_size(&t, (2 * pa) + 1)) != MP_OKAY) {
      return err;
   }

   /* default used is maximum possible size */
   t.used = (2 * pa) + 1;

   for (ix = 0; ix < pa; ix++) {
      mp_digit u;
      int iy;

      /* first calculate the digit at 2*ix */
      /* calculate double precision result */
      mp_word r = (mp_word)t.dp[2*ix] +
                  ((mp_word)a->dp[ix] * (mp_word)a->dp[ix]);

      /* store lower part in result */
      t.dp[ix+ix] = (mp_digit)(r & (mp_word)MP_MASK);

      /* get the carry */
      u           = (mp_digit)(r >> (mp_word)MP_DIGIT_BIT);

      for (iy = ix + 1; iy < pa; iy++) {
         /* first calculate the product */
         r       = (mp_word)a->dp[ix] * (mp_word)a->dp[iy];

         /* now calculate the double precision result, note we use
          * addition instead of *2 since it's easier to optimize.
          */
         /* Some architectures and/or compilers seem to prefer a bit-shift nowadays */
         r       = (mp_word)t.dp[ix + iy] + (r<<1) + (mp_word)u;

         /* store lower part */
         t.dp[ix + iy] = (mp_digit)(r & (mp_word)MP_MASK);

         /* get carry */
         u       = (mp_digit)(r >> (mp_word)MP_DIGIT_BIT);
      }
      /* propagate upwards */
      while (u != 0uL) {
         mp_digit tmp;
         /*
            "u" can get bigger than MP_DIGIT_MAX and would need a bigger type
            for the sum (mp_word). That is costly if mp_word is not a native
            integer but a bigint from the compiler library. We do a manual
            multiword addition instead.
          */
         /* t.dp[ix + iy] has been masked off by MP_MASK and is hence of the correct size
            and we can just add the lower part of "u". Carry is guaranteed to fit into
            the type used for mp_digit, too, so we can extract it later. */
         tmp = t.dp[ix + iy] + (u & MP_MASK);
         /* t.dp[ix + iy] is set to the result minus the carry, carry is still in "tmp" */
         t.dp[ix + iy] = tmp & MP_MASK;
         /* Add high part of "u" and the carry from "tmp" to get the next "u" */
         u = (u >> MP_DIGIT_BIT) + (tmp >> MP_DIGIT_BIT);
         ++iy;
      }
   }

   mp_clamp(&t);
   mp_exch(&t, b);
   mp_clear(&t);
   return MP_OKAY;
}
#endif
