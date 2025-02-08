#include "tommath_private.h"
#ifdef MP_REDUCE_2K_SETUP_L_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* determines the setup value */
mp_err mp_reduce_2k_setup_l(const mp_int *a, mp_int *d)
{
   mp_err err = MP_OKAY;
   mp_int tmp;

   if ((err = mp_init(&tmp)) != MP_OKAY)                                 MP_TRACE_ERROR(err, LTM_ERR);

   if ((err = mp_2expt(&tmp, mp_count_bits(a))) != MP_OKAY)              MP_TRACE_ERROR(err, LTM_ERR_1);

   if ((err = s_mp_sub(&tmp, a, d)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR_1);

LTM_ERR_1:
   mp_clear(&tmp);
LTM_ERR:
   return err;
}
#endif
