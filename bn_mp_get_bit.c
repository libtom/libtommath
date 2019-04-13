#include "tommath_private.h"
#ifdef BN_MP_GET_BIT_C

/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Checks the bit at position b and returns MP_YES
   if the bit is 1, MP_NO if it is 0 and MP_VAL
   in case of error */
int mp_get_bit(const mp_int *a, int b)
{
   int limb;
   mp_digit bit, isset;

   if (b < 0) {
      return MP_VAL;
   }

   limb = b / MP_DIGIT_BIT;

   if (limb >= a->used) {
      return MP_NO;
   }

   bit = (mp_digit)(1) << (b % MP_DIGIT_BIT);

   isset = a->dp[limb] & bit;
   return (isset != 0u) ? MP_YES : MP_NO;
}

#endif
