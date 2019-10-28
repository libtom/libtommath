#include "tommath_private.h"
#ifdef MP_REDUCE_IS_2K_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* determines if mp_reduce_2k can be used */
bool mp_reduce_is_2k(const mp_int *a)
{
   if (a->used == 0) {
      return false;
   } else if (a->used == 1) {
      return true;
   } else if (a->used > 1) {
      size_t ix, iy, iw;
      mp_digit iz;
      iy = mp_count_bits(a);
      iz = 1;
      iw = 1;

      /* Test every bit from the second digit up, must be 1 */
      for (ix = MP_DIGIT_BIT; ix < iy; ix++) {
         if ((a->dp[iw] & iz) == 0u) {
            return false;
         }
         iz <<= 1;
         if (iz > MP_DIGIT_MAX) {
            ++iw;
            iz = 1;
         }
      }
      return true;
   } else {
      return true;
   }
}

#endif
