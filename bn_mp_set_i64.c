#include "tommath_private.h"
#ifdef BN_MP_SET_I64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* Set the least signficant 64 bit of "a" to b, signed */
void mp_set_i64(mp_int *a, int64_t b)
{
   mp_set_u64(a, (b < 0) ? -(uint64_t)b : (uint64_t)b);
   if (b < 0) {
      a->sign = MP_NEG;
   }
}


#endif
