#include "tommath_private.h"
#ifdef BN_MP_ZERO_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* set to zero */
void mp_zero(mp_int *a)
{
   int       n;
   mp_digit *tmp;

   a->sign = MP_ZPOS;
   a->used = 0;

   tmp = a->dp;
   for (n = 0; n < a->alloc; n++) {
      *tmp++ = 0;
   }
}
#endif
