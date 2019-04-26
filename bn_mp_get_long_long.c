#include "tommath_private.h"
#ifdef BN_MP_GET_LONG_LONG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* get the lower unsigned long long of an mp_int, platform dependent */
unsigned long long mp_get_long_long(const mp_int *a)
{
   int i;
   unsigned long long res;

   if (MP_IS_ZERO(a)) {
      return 0;
   }

   /* get number of digits of the lsb we have to read */
   i = MP_MIN(a->used, (((CHAR_BIT * (int)sizeof(unsigned long long)) + MP_DIGIT_BIT - 1) / MP_DIGIT_BIT)) - 1;

   /* get most significant digit of result */
   res = (unsigned long long)a->dp[i];

#if MP_DIGIT_BIT < 64
   while (--i >= 0) {
      res = (res << MP_DIGIT_BIT) | (unsigned long long)a->dp[i];
   }
#endif
   return res;
}
#endif
