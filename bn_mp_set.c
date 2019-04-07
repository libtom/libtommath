#include "tommath_private.h"
#ifdef BN_MP_SET_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* set to a digit */
void mp_set(mp_int *a, mp_digit b)
{
   mp_zero(a);
   a->dp[0] = b & MP_MASK;
   a->used  = (a->dp[0] != 0u) ? 1 : 0;
}
#endif
