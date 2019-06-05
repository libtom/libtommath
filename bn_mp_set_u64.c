#include "tommath_private.h"
#ifdef BN_MP_SET_U64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* Set the least signficant 64 bit of "a" to b, unsigned */
void mp_set_u64(mp_int *a, uint64_t b)
{
   int i = 0;
   while (b != 0u) {
      a->dp[i++] = ((mp_digit)b & MP_MASK);
      if (64 <= MP_DIGIT_BIT) {
         break;
      }
      b >>= ((64 <= MP_DIGIT_BIT) ? 0 : MP_DIGIT_BIT);
   }
   a->used = i;
   a->sign = MP_ZPOS;
   MP_ZERO_DIGITS(a->dp + a->used, a->alloc - a->used);
}


#endif
