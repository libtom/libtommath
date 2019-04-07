#include "tommath_private.h"
#ifdef BN_MP_SET_INT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* set a 32-bit const */
int mp_set_int(mp_int *a, unsigned long b)
{
   return mp_set_long(a, b & 0xFFFFFFFFUL);
}
#endif
