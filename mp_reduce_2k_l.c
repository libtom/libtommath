#include "tommath_private.h"
#ifdef MP_REDUCE_2K_L_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* reduces a modulo n where n is of the form 2**p - d
   This differs from reduce_2k since "d" can be larger
   than a single digit.
*/
mp_err mp_reduce_2k_l(mp_int *a, const mp_int *n, const mp_int *d)
{
   mp_int q;
   mp_err err = MP_OKAY;
   int    p;

   if ((err = mp_init(&q)) != MP_OKAY)                                   MP_TRACE_ERROR(err, LTM_ERR);

   p = mp_count_bits(n);

   for (;;) {
      /* q = a/2**p, a = a mod 2**p */
      if ((err = mp_div_2d(a, p, &q, a)) != MP_OKAY)                     MP_TRACE_ERROR(err, LTM_ERR_1);

      /* q = q * d */
      if ((err = mp_mul(&q, d, &q)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR_1);

      /* a = a + q */
      if ((err = s_mp_add(a, &q, a)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR_1);

      if (mp_cmp_mag(a, n) == MP_LT) {
         break;
      }
      if ((err = s_mp_sub(a, n, a)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR_1);
   }

LTM_ERR_1:
   mp_clear(&q);
LTM_ERR:
   return err;
}

#endif
