#include "tommath_private.h"
#ifdef BN_MP_SET_I32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* Set the least signficant 32 bit of "a" to b, signed */
void mp_set_i32(mp_int *a, int32_t b)
{
   mp_set_u32(a, (b < 0) ? -(uint32_t)b : (uint32_t)b);
   if (b < 0) {
      a->sign = MP_NEG;
   }
}
#endif
