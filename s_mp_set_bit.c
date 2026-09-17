#include "tommath_private.h"
#ifdef S_MP_SET_BIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

mp_err s_mp_set_bit(mp_int *a, int b)
{
   mp_digit bit;
   int limb = b / MP_DIGIT_BIT;

   if (limb < 0 || limb >= a->used) {
      return MP_VAL;
   }

   bit = (mp_digit)1 << (b % MP_DIGIT_BIT);
   a->dp[limb] |= bit;
   return MP_OKAY;
}


#endif
