#include "tommath_private.h"
#ifdef BN_MP_COUNT_BITS_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* returns the number of bits in an int */
mp_err mp_count_bits(const mp_int *a, size_t *size)
{
   size_t   r;
   mp_digit q;

   *size = 0;

   /* shortcut */
   if (MP_IS_ZERO(a)) {
      return MP_OKAY;
   }

   /* get number of digits and add that */
   r = a->used;
   if (r > (SIZE_MAX/MP_DIGIT_BIT)) {
      return MP_BUF;
   }
   r = (r - 1u) * MP_DIGIT_BIT;

   /* take the last digit and count the bits in it */
   q = a->dp[a->used - 1];
   while (q > 0u) {
      ++r;
      q >>= 1u;
   }

   *size = r;

   return MP_OKAY;
}
#endif
