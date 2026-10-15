#include "tommath_private.h"
#ifdef S_MP_NTT_INV2_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
void s_mp_ntt_inv2(const ntt_mont *mc, mp_digit *b, int L, int step, const ntt_sroots *R)
{
   const mp_digit *w = R->w;
   int m = R->m, e = 0, j;

   for (j = 0; j < L; j++) {
      mp_digit y0 = b[j];
      mp_digit y1 = s_mp_ntt_mont_mul(mc, b[j + L], w[e ? m - e : 0]);
      b[j]     = s_mp_ntt_mont_add(mc, y0, y1);
      b[j + L] = s_mp_ntt_mont_sub(mc, y0, y1);
      e += step;
      if (e >= m) {
         e -= m;
      }
   }
}
#endif



#endif
