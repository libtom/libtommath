#include "tommath_private.h"
#ifdef BN_MP_COMPLEMENT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* b = ~a */
int mp_complement(const mp_int *a, mp_int *b)
{
   int res = mp_neg(a, b);
   return (res == MP_OKAY) ? mp_sub_d(b, 1uL, b) : res;
}
#endif
