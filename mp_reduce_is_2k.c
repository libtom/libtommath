#include "tommath_private.h"
#ifdef MP_REDUCE_IS_2K_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* determines if mp_reduce_2k can be used */
bool mp_reduce_is_2k(const mp_int *a)
{
   if (mp_iszero(a)) {
      return false;
   } else if (a->used == 1) {
      return true;
   } else if (a->used > 1) {
      int ix, iy, iw = 1;
      mp_digit iz;
      /* Algorithm as implemented does not work if the least significant digit is zero */
      iz = a->dp[0] & MP_MASK;
      if (iz == 0u) {
         return false;
      }

      iy = mp_count_bits(a);
      iz = 1;
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
