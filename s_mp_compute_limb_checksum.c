#include "tommath_private.h"
#ifdef S_MP_COMPUTE_LIMB_CHECKSUM_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

mp_word s_mp_compute_limb_checksum(const mp_int *a)
{
   mp_word sum = 0u, carry;
   int i;

   if (mp_iszero(a)) {
      return 0u;
   }

   for (i = 0; i < a->used; ++i) {
      sum += a->dp[i];
      carry = sum >> MP_DIGIT_BIT;
      sum = (sum & MP_MASK) + carry;
   }

   if (sum == (mp_word)MP_MASK) {
      sum = 0u;
   }

   return sum;
}


#endif
