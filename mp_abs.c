#include "tommath_private.h"
#ifdef MP_ABS_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* b = |a|
 *
 * Simple function copies the input and fixes the sign to positive
 */
mp_err mp_abs(const mp_int *a, mp_int *b)
{
   mp_err err = MP_OKAY;

   /* copy a to b */
   if ((err = mp_copy(a, b)) != MP_OKAY)                       MP_TRACE_ERROR(err, LTM_ERR);

   /* force the sign of b to positive */
   b->sign = MP_ZPOS;

LTM_ERR:
   return err;
}
#endif
