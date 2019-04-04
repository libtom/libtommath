#include "tommath_private.h"
#ifdef BN_MP_SET_LONG_C
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

/* set a platform dependent unsigned long int */
#if (ULONG_MAX != 0xFFFFFFFFUL) || (DIGIT_BIT < 32)
MP_SET_XLONG(mp_set_long, unsigned long)
#else
int mp_set_long(mp_int *a, unsigned long b)
{
   int x = 0;
   int res = mp_grow(a, (CHAR_BIT * sizeof(unsigned long) + DIGIT_BIT - 1) / DIGIT_BIT);
   if (res == MP_OKAY) {
      mp_zero(a);
      if (b) {
         a->dp[x++] = (mp_digit)b;
      }
      a->used = x;
   }
   return res;
}

#endif
#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
