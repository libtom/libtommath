#include "tommath_private.h"
#ifdef MP_ADD_D_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* single digit addition */
mp_err mp_add_d(const mp_int *a, mp_digit b, mp_int *c)
{
   mp_err err = MP_OKAY;
   int oldused;

   /* fast path for a == c */
   if (a == c) {
      if (!mp_isneg(c) &&
          !mp_iszero(c) &&
          ((c->dp[0] + b) < MP_DIGIT_MAX)) {
         c->dp[0] += b;
         return MP_OKAY;
      }
      if (mp_isneg(c) &&
          (c->dp[0] > b)) {
         c->dp[0] -= b;
         return MP_OKAY;
      }
   }

   /* grow c as required */
   if ((err = mp_grow(c, a->used + 1)) != MP_OKAY)                       MP_TRACE_ERROR(err, LTM_ERR);

   /* if a is negative and |a| >= b, call c = |a| - b */
   if (mp_isneg(a) && ((a->used > 1) || (a->dp[0] >= b))) {
      mp_int a_ = *a;
      /* temporarily fix sign of a */
      a_.sign = MP_ZPOS;

      /* c = |a| - b */
      if ((err = mp_sub_d(&a_, b, c)) != MP_OKAY)                       MP_TRACE_ERROR(err, LTM_ERR);

      /* fix sign  */
      c->sign = MP_NEG;

      /* clamp */
      mp_clamp(c);

      return err;
   }

   /* old number of used digits in c */
   oldused = c->used;

   /* if a is positive */
   if (!mp_isneg(a)) {
      /* add digits, mu is carry */
      int i;
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
   s_mp_zero_digs(c->dp + c->used, oldused - c->used);
   mp_clamp(c);

LTM_ERR:
   return err;
}

#endif
