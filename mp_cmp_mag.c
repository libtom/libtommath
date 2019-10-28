#include "tommath_private.h"
#ifdef MP_CMP_MAG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* compare maginitude of two ints (unsigned) */
mp_ord mp_cmp_mag(const mp_int *a, const mp_int *b)
{
   size_t n;

   /* compare based on # of non-zero digits */
   if (a->used != b->used) {
      return a->used > b->used ? MP_GT : MP_LT;
   }

   /* compare based on digits  */
   for (n = a->used; n --> 0;) {
      mp_digit x = a->dp[n], y = b->dp[n];
      if (x != y) {
         return x > y ? MP_GT : MP_LT;
      }
   }

   return MP_EQ;
}
#endif
