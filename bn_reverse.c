#include "tommath_private.h"
#ifdef BN_REVERSE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* reverse an array, used for radix code */
void bn_reverse(unsigned char *s, int len)
{
   int     ix, iy;
   unsigned char t;

   ix = 0;
   iy = len - 1;
   while (ix < iy) {
      t     = s[ix];
      s[ix] = s[iy];
      s[iy] = t;
      ++ix;
      --iy;
   }
}
#endif
