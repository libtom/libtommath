#include "tommath_private.h"
#ifdef S_MP_NTT_SMOOTH_LENGTH_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#if (MP_DIGIT_BIT > 16)
/* TODO: there are no alternatives anymore, pull the two together */
int s_mp_ntt_smooth_length(int need)
{
   int length;
   length = s_mp_ntt_smooth_length_table(need);
   return length;
}
#endif

#endif
