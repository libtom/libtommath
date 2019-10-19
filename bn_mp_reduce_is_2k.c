#include "tommath_private.h"
#ifdef BN_MP_REDUCE_IS_2K_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* determines if mp_reduce_2k can be used */
mp_bool mp_reduce_is_2k(const mp_int *a)
{
   size_t ix, iy, iw;
   mp_digit iz;
   mp_err err;

   if (a->used == 0) {
      return MP_NO;
   } else if (a->used == 1) {
      return MP_YES;
   } else if (a->used > 1) {
      /* TODO: make this function return an mp_err and put the result in e.g.: mp_bool *can_be_used*/
      if ((err = mp_count_bits(a, &iy)) != MP_OKAY) {
         /* Nothing, but see TODO above */
      }
      iz = 1;
      iw = 1;

      /* Test every bit from the second digit up, must be 1 */
      for (ix = MP_DIGIT_BIT; ix < iy; ix++) {
         if ((a->dp[iw] & iz) == 0u) {
            return MP_NO;
         }
         iz <<= 1;
         if (iz > MP_DIGIT_MAX) {
            ++iw;
            iz = 1;
         }
      }
      return MP_YES;
   } else {
      return MP_YES;
   }
}

#endif
