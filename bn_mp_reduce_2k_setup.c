#include "tommath_private.h"
#ifdef BN_MP_REDUCE_2K_SETUP_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* determines the setup value */
mp_err mp_reduce_2k_setup(const mp_int *a, mp_digit *d)
{
   mp_err err;
   mp_int tmp;
   int    p;
   size_t size_a;

   if ((err = mp_init(&tmp)) != MP_OKAY) {
      return err;
   }
   if ((err = mp_count_bits(a, &size_a)) != MP_OKAY) {
      return err;
   }
   /* TODO: can be skipped when all relevant functions accept size_t */
   if (size_a > INT_MAX) {
      return MP_VAL;
   }
   p = (int)size_a;

   if ((err = mp_2expt(&tmp, p)) != MP_OKAY) {
      mp_clear(&tmp);
      return err;
   }

   if ((err = s_mp_sub(&tmp, a, &tmp)) != MP_OKAY) {
      mp_clear(&tmp);
      return err;
   }

   *d = tmp.dp[0];
   mp_clear(&tmp);
   return MP_OKAY;
}
#endif
