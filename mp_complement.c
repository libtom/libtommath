#include "tommath_private.h"
#ifdef MP_COMPLEMENT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* b = ~a */
mp_err mp_complement(const mp_int *a, mp_int *b)
{
   mp_err err = MP_OKAY;
   mp_int a_ = *a;
   a_.sign = ((a_.sign == MP_ZPOS) && !mp_iszero(a)) ? MP_NEG : MP_ZPOS;
   if ((err = mp_sub_d(&a_, 1uL, b)) != MP_OKAY)                        MP_TRACE_ERROR(err, LTM_ERR);
LTM_ERR:
   return err;
}
#endif
