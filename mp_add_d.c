#include "tommath_private.h"
#ifdef MP_ADD_D_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* single digit addition */
mp_err mp_add_d(const mp_int *a, mp_digit b, mp_int *c)
{
   size_t oldused;

   /* fast path for a == c */
   if (a == c) {
      if ((c->sign == MP_ZPOS) &&
          !mp_iszero(c) &&
          ((c->dp[0] + b) < MP_DIGIT_MAX)) {
         c->dp[0] += b;
         return MP_OKAY;
      }
      if ((c->sign == MP_NEG) &&
          (c->dp[0] > b)) {
         c->dp[0] -= b;
         return MP_OKAY;
      }
   }

   /* grow c as required */
   if (c->alloc < (a->used + 1)) {
      mp_err err;
      if ((err = mp_grow(c, a->used + 1)) != MP_OKAY) {
         return err;
      }
   }

   /* if a is negative and |a| >= b, call c = |a| - b */
   if ((a->sign == MP_NEG) && ((a->used > 1) || (a->dp[0] >= b))) {
      mp_err err;
      mp_int a_ = *a;
      /* temporarily fix sign of a */
      a_.sign = MP_ZPOS;

      /* c = |a| - b */
      err = mp_sub_d(&a_, b, c);

      /* fix sign  */
      c->sign = MP_NEG;

      /* clamp */
      mp_clamp(c);

      return err;
   }

   /* old number of used digits in c */
   oldused = c->used;

   /* if a is positive */
   if (a->sign == MP_ZPOS) {
      /* add digits, mu is carry */
      size_t i;
      mp_digit mu = b;
      for (i = 0; i < a->used; i++) {
         c->dp[i] = a->dp[i] + mu;
         mu = c->dp[i] >> MP_DIGIT_BIT;
         c->dp[i] &= MP_MASK;
      }
      /* set final carry */
      c->dp[i] = mu;

      /* setup size */
      c->used = a->used + 1;
   } else {
      /* a was negative and |a| < b */
      c->used = 1;

      /* the result is a single digit */
      c->dp[0] = (a->used == 1) ? b - a->dp[0] : b;
   }

   /* sign always positive */
   c->sign = MP_ZPOS;

   /* now zero to oldused */
   MP_ZERO_DIGITS_NEW(c->dp + c->used, c->dp + oldused);
   mp_clamp(c);

   return MP_OKAY;
}

#endif
