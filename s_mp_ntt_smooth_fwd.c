#include "tommath_private.h"
#ifdef S_MP_NTT_SMOOTH_FWD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */




/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
void s_mp_ntt_smooth_fwd(ntt_mont *mc, mp_digit *a, int m, const ntt_sroots *R)
{
   int k, start, r, ell, L, step;
   mp_digit *b;

   for (k = 0; k < R->nstages; k++) {
      r = R->radix[k];
      ell = R->ell[k];
      L = ell / r;
      step = m / ell;
      for (start = 0; start < m; start += ell) {
         b = a + start;
         switch (r) {
         case 5:
            s_mp_ntt_fwd5(mc, b, L, step, R);
            break;
         case 3:
            s_mp_ntt_fwd3(mc, b, L, step, R);
            break;
         default:
            s_mp_ntt_fwd2(mc, b, L, step, R);
            break;
         }
      }
   }
}
#endif




#endif
