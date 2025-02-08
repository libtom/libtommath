#include "tommath_private.h"
#ifdef MP_MULMOD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* d = a * b (mod c) */
mp_err mp_mulmod(const mp_int *a, const mp_int *b, const mp_int *c, mp_int *d)
{
   mp_err err = MP_OKAY;
   if ((err = mp_mul(a, b, d)) != MP_OKAY)                               MP_TRACE_ERROR(err, LTM_ERR);
   if ((err = mp_mod(d, c, d)) != MP_OKAY)                              MP_TRACE_ERROR(err, LTM_ERR);
LTM_ERR:
   return err;
}
#endif
