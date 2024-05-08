#include "tommath_private.h"
#ifdef MP_RAND_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

mp_err mp_rand(mp_int *a, int digits)
{
   int i;
   mp_err err = MP_OKAY;

   mp_zero(a);

   if (digits <= 0) {
      return MP_OKAY;
   }

   if ((err = mp_grow(a, digits)) != MP_OKAY)                            MP_TRACE_ERROR(err, LTM_ERR);

   if ((err = s_mp_rand_source(a->dp, (size_t)digits * sizeof(mp_digit))) != MP_OKAY)
      MP_TRACE_ERROR(err, LTM_ERR);

   /* TODO: We ensure that the highest digit is nonzero. Should this be removed? */
   while ((a->dp[digits - 1] & MP_MASK) == 0u) {
      if ((err = s_mp_rand_source(a->dp + digits - 1, sizeof(mp_digit))) != MP_OKAY)
         MP_TRACE_ERROR(err, LTM_ERR);
   }

   a->used = digits;
   for (i = 0; i < digits; ++i) {
      a->dp[i] &= MP_MASK;
   }

LTM_ERR:
   return err;
}
#endif
