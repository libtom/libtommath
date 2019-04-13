#include "tommath_private.h"
#ifdef BN_MP_GET_LONG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* get the lower unsigned long of an mp_int, platform dependent */
unsigned long mp_get_long(const mp_int *a)
{
   int i;
   unsigned long res;

   if (MP_IS_ZERO(a)) {
      return 0;
   }

   /* get number of digits of the lsb we have to read */
   i = MP_MIN(a->used, (((CHAR_BIT * (int)sizeof(unsigned long)) + MP_DIGIT_BIT - 1) / MP_DIGIT_BIT)) - 1;

   /* get most significant digit of result */
   res = (unsigned long)a->dp[i];

#if (ULONG_MAX != 0xFFFFFFFFUL) || (MP_DIGIT_BIT < 32)
   while (--i >= 0) {
      res = (res << MP_DIGIT_BIT) | (unsigned long)a->dp[i];
   }
#endif
   return res;
}
#endif
