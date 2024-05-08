#include "tommath_private.h"
#ifdef MP_NEG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* b = -a */
mp_err mp_neg(const mp_int *a, mp_int *b)
{
   mp_err err = MP_OKAY;

   if ((err = mp_copy(a, b)) != MP_OKAY)                                 MP_TRACE_ERROR(err, LTM_ERR);
   b->sign = ((!mp_iszero(b) && !mp_isneg(b)) ? MP_NEG : MP_ZPOS);

LTM_ERR:
   return err;
}
#endif
