#include "tommath_private.h"
#ifdef S_MP_STR_REVERSE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



void s_mp_str_reverse(char *s, int len)
{
   int x = 0, y = len - 1;
   char t;

   while (x < y) {
      t = s[x];
      s[x] = s[y];
      s[y] = t;
      x++;
      y--;
   }
}


#endif
