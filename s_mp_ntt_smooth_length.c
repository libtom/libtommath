#include "tommath_private.h"
#ifdef S_MP_NTT_SMOOTH_LENGTH_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
int s_mp_ntt_smooth_length(int need)
{
   int length;
   length = s_mp_ntt_smooth_length_weighted(need, NTT_SMOOTH_W2, NTT_SMOOTH_W3, NTT_SMOOTH_W5);
   return length;
}
#endif


#endif
