#include "tommath_private.h"
#ifdef BN_MP_SET_U32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* Set the least signficant 32 bit of "a" to b, unsigned */
void mp_set_u32(mp_int *a, uint32_t b)
{
   int i = 0;
   while (b != 0u) {
      a->dp[i++] = ((mp_digit)b & MP_MASK);
      if (32 <= MP_DIGIT_BIT) {
         break;
      }
      b >>= ((32 <= MP_DIGIT_BIT) ? 0 : MP_DIGIT_BIT);
   }
   a->used = i;
   a->sign = MP_ZPOS;
   MP_ZERO_DIGITS(a->dp + a->used, a->alloc - a->used);
}
#endif
