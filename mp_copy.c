#include "tommath_private.h"
#ifdef MP_COPY_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* copy, b = a */
mp_err mp_copy(const mp_int *a, mp_int *b)
{
   int n;

   /* if dst == src do nothing */
   if (a == b) {
      return MP_OKAY;
   }

   /* grow dest */
   if (b->alloc < a->used) {
      mp_err err;
      if ((err = mp_grow(b, a->used)) != MP_OKAY) {
         return err;
      }
   }

   /* zero b and copy the parameters over */

   /* copy all the digits */
   for (n = 0; n < a->used; n++) {
      b->dp[n] = a->dp[n];
   }

   /* clear high digits */
   MP_ZERO_DIGITS(b->dp + a->used, b->used - a->used);

   /* copy used count and sign */
   b->used = a->used;
   b->sign = a->sign;
   return MP_OKAY;
}
#endif
