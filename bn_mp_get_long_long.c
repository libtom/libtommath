#include "tommath_private.h"
#ifdef BN_MP_GET_LONG_LONG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * SPDX-License-Identifier: Unlicense
 */

/* get the lower unsigned long long of an mp_int, platform dependent */
unsigned long long mp_get_long_long(const mp_int *a)
{
   int i;
   unsigned long long res;

   if (IS_ZERO(a)) {
      return 0;
   }

   /* get number of digits of the lsb we have to read */
   i = MIN(a->used, (((CHAR_BIT * (int)sizeof(unsigned long long)) + DIGIT_BIT - 1) / DIGIT_BIT)) - 1;

   /* get most significant digit of result */
   res = (unsigned long long)a->dp[i];

#if DIGIT_BIT < 64
   while (--i >= 0) {
      res = (res << DIGIT_BIT) | (unsigned long long)a->dp[i];
   }
#endif
   return res;
}
#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
