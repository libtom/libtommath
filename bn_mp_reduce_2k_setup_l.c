#include "tommath_private.h"
#ifdef BN_MP_REDUCE_2K_SETUP_L_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* determines the setup value */
int mp_reduce_2k_setup_l(const mp_int *a, mp_int *d)
{
   int    res;
   mp_int tmp;

   if ((res = mp_init(&tmp)) != MP_OKAY) {
      return res;
   }

   if ((res = mp_2expt(&tmp, mp_count_bits(a))) != MP_OKAY) {
      goto LBL_ERR;
   }

   if ((res = s_mp_sub(&tmp, a, d)) != MP_OKAY) {
      goto LBL_ERR;
   }

LBL_ERR:
   mp_clear(&tmp);
   return res;
}
#endif
