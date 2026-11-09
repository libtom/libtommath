#include "tommath_private.h"
#ifdef S_MP_NTT_SMOOTH_LENGTH_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
int s_mp_ntt_smooth_length(int need)
{
   int length;
   length = s_mp_ntt_smooth_length_table(need);
   return length;
}
#endif
#endif
