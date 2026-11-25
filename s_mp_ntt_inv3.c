#include "tommath_private.h"
#ifdef S_MP_NTT_INV3_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
void s_mp_ntt_inv3(const ntt_mont *mc, mp_digit *b, int L, int step, const ntt_sroots *R)
{
   const mp_digit *w = R->w;
   int m = R->m, j;
   int s1 = step, s2 = 2 * step;
   int e1 = 0, e2 = 0;
   mp_digit t = R->c3i;

   for (j = 0; j < L; j++) {
      mp_digit y0 = b[j];
      mp_digit y1 = s_mp_ntt_mont_mul(mc, b[j + L],     w[e1 ? m - e1 : 0]);
      mp_digit y2 = s_mp_ntt_mont_mul(mc, b[j + 2 * L], w[e2 ? m - e2 : 0]);
      mp_digit v  = s_mp_ntt_mont_mul(mc, s_mp_ntt_mont_sub(mc, y1, y2), t);

      b[j]         = s_mp_ntt_mont_add(mc, s_mp_ntt_mont_add(mc, y0, y1), y2);
      b[j + L]     = s_mp_ntt_mont_add(mc, s_mp_ntt_mont_sub(mc, y0, y2), v);
      b[j + 2 * L] = s_mp_ntt_mont_sub(mc, s_mp_ntt_mont_sub(mc, y0, y1), v);
      e1 += s1;
      if (e1 >= m) {
         e1 -= m;
      }
      e2 += s2;
      if (e2 >= m) {
         e2 -= m;
      }
   }
}
#endif



#endif
