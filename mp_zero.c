#include "tommath_private.h"
#ifdef MP_ZERO_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* set to zero */
void mp_zero(mp_int *a)
{
   a->sign = MP_ZPOS;
   a->used = 0;
   MP_ZERO_DIGITS_NEW(a->dp, a->dp + a->alloc);
}
#endif
