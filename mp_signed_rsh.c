#include "tommath_private.h"
#ifdef MP_SIGNED_RSH_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* shift right by a certain bit count with sign extension */
mp_err mp_signed_rsh(const mp_int *a, int b, mp_int *c)
{
   mp_err err = MP_OKAY;
   if (!mp_isneg(a)) {
      if ((err = mp_div_2d(a, b, c, NULL)) != MP_OKAY)                  MP_TRACE_ERROR(err, LTM_ERR);
      return err;
   }

   if ((err = mp_add_d(a, 1uL, c)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR);

   if ((err = mp_div_2d(c, b, c, NULL)) != MP_OKAY)                     MP_TRACE_ERROR(err, LTM_ERR);
   if ((err = mp_sub_d(c, 1uL, c)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR);

LTM_ERR:
   return err;
}
#endif
