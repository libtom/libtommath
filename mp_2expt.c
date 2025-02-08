#include "tommath_private.h"
#ifdef MP_2EXPT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* computes a = 2**b
 *
 * Simple algorithm which zeroes the int, grows it then just sets one bit
 * as required.
 */
mp_err mp_2expt(mp_int *a, int b)
{
   mp_err err = MP_OKAY;

   if (b < 0) {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   /* zero a as per default */
   mp_zero(a);

   /* grow a to accommodate the single bit */
   if ((err = mp_grow(a, (b / MP_DIGIT_BIT) + 1)) != MP_OKAY)            MP_TRACE_ERROR(err, LTM_ERR);

   /* set the used count of where the bit will go */
   a->used = (b / MP_DIGIT_BIT) + 1;

   /* put the single bit in its place */
   a->dp[b / MP_DIGIT_BIT] = (mp_digit)1 << (mp_digit)(b % MP_DIGIT_BIT);

LTM_ERR:
   return err;
}
#endif
