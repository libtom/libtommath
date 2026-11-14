#include "tommath_private.h"
#ifdef S_MP_NTT_INVMOD_D_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


mp_digit s_mp_ntt_invmod_d(mp_digit p)
{
   mp_digit x = 1;
   int i;
   for (i = 0; i < 6; i++) {
      x = x * (mp_digit)(2 - p * x);
   }
   return x;
}

#endif
