#include "tommath_private.h"
#ifdef S_MP_NTT_ILOG2_CEIL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
int s_mp_ntt_ilog2_ceil(int x)
{
   int s = 0;
   while ((1 << s) < x) {
      s++;
   }
   return s;
}
#endif


#endif
