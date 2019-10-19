#include "tommath_private.h"
#ifdef BN_MP_REDUCE_2K_SETUP_L_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* determines the setup value */
mp_err mp_reduce_2k_setup_l(const mp_int *a, mp_int *d)
{
   mp_err err;
   mp_int tmp;
   size_t size_a;

   if ((err = mp_init(&tmp)) != MP_OKAY) {
      return err;
   }
   if ((err = mp_count_bits(a, &size_a)) != MP_OKAY)          goto LBL_ERR;
   /* TODO: can be skipped when all relevant functions accept size_t */
   if (size_a > INT_MAX) {
      return MP_VAL;
   }
   if ((err = mp_2expt(&tmp, (int)size_a)) != MP_OKAY)        goto LBL_ERR;

   if ((err = s_mp_sub(&tmp, a, d)) != MP_OKAY) {
      goto LBL_ERR;
   }

LBL_ERR:
   mp_clear(&tmp);
   return err;
}
#endif
