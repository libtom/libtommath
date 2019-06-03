#include "tommath_private.h"
#ifdef BN_MP_GET_MAG64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* Return the absolute value of the least significant 64 bits */
uint64_t mp_get_mag64(const mp_int *a)
{
   unsigned i = MP_MIN((unsigned)a->used, (unsigned)((64 + MP_DIGIT_BIT - 1) / MP_DIGIT_BIT));
   uint64_t res = 0uL;
   while (i-- > 0) {
      res <<= ((64 <= MP_DIGIT_BIT) ? 0 : MP_DIGIT_BIT);
      res |= (uint64_t)a->dp[i];
      if (64 <= MP_DIGIT_BIT) {
         break;
      }
   }
   return res;
}


#endif
