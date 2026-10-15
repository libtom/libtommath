#include "tommath_private.h"
#ifdef S_MP_NTT_SMOOTH_INV_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
void s_mp_ntt_smooth_inv(ntt_mont *mc, mp_digit *a, int m, const ntt_sroots *R)
{
   int k, start, i, r, ell, L, step;
   mp_digit ninv;
   mp_digit *b;

   for (k = R->nstages - 1; k >= 0; k--) {
      r = R->radix[k];
      ell = R->ell[k];
      L = ell / r;
      step = m / ell;
      for (start = 0; start < m; start += ell) {
         b = a + start;
         switch (r) {
         case 5:
            s_mp_ntt_inv5(mc, b, L, step, R);
            break;
         case 3:
            s_mp_ntt_inv3(mc, b, L, step, R);
            break;
         default:
            s_mp_ntt_inv2(mc, b, L, step, R);
            break;
         }
      }
   }
   ninv = s_mp_ntt_mont_pow(mc, s_mp_ntt_to_mont(mc, (mp_digit)m), mc->p - 2);   /* 1/m, Montgomery */
   for (i = 0; i < m; i++) {
      a[i] = s_mp_ntt_mont_mul(mc, a[i], ninv);
   }
}
#endif



#endif
