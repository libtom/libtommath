#include "tommath_private.h"
#ifdef MP_RADIX_SIZE_OVERESTIMATE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

mp_err mp_radix_size_overestimate(const mp_int *a, const int radix, size_t *size)
{
   mp_err err = MP_OKAY;
   if (MP_HAS(S_MP_RADIX_SIZE_OVERESTIMATE)) {
      if ((err = s_mp_radix_size_overestimate(a, radix, size)) != MP_OKAY)        MP_TRACE_ERROR(err, LTM_ERR);
      return err;
   }
   if (MP_HAS(MP_RADIX_SIZE)) {
      if ((err = mp_radix_size(a, radix, size)) != MP_OKAY)                       MP_TRACE_ERROR(err, LTM_ERR);
      return err;
   }
   err = MP_ERR;
LTM_ERR:
   return err;
}

#endif
