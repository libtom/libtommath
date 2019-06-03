#include "tommath_private.h"
#ifdef BN_MP_GET_MAG32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* Return the absolute value of the least significant 32 bits */
uint32_t mp_get_mag32(const mp_int *a)
{
   unsigned i = MP_MIN((unsigned)a->used, (unsigned)((32 + MP_DIGIT_BIT - 1) / MP_DIGIT_BIT));
   uint32_t res = 0uL;
   while (i-- > 0) {
      res <<= ((32 <= MP_DIGIT_BIT) ? 0 : MP_DIGIT_BIT);
      res |= (uint32_t)a->dp[i];
      if (32 <= MP_DIGIT_BIT) {
         break;
      }
   }
   return res;
}
#endif
