#include "tommath_private.h"
#ifdef MP_LOG_N_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

mp_err mp_log_n(const mp_int *a, int base, int *c)
{
   mp_int b;
   mp_err err;

   if ((err = mp_init_i32(&b, base)) != MP_OKAY)                                                         goto LTM_ERR;
   if ((err = mp_log(a, &b, c)) != MP_OKAY)                                                             goto LTM_ERR;

LTM_ERR:
   mp_clear(&b);
   return err;
}

#endif
