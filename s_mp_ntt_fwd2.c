#include "tommath_private.h"
#ifdef S_MP_NTT_FWD2_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/*
    Forward: transform first and do the twiddles after.
    Inverse: undoes the twiddles first (reading the same table at m - e,
    since omega^{-e} = omega^{m-e}) and inverse transform after.
    The inverse is unscaled, because s_mp_ntt_smooth_inv divides by m once at
    the end. Slot q carries index q in both directions.
 */
void s_mp_ntt_fwd2(const ntt_mont *mc, mp_digit *b, int L, int step, const ntt_sroots *R)
{
   const mp_digit *w = R->w;
   int m = R->m, e = 0, j;

   for (j = 0; j < L; j++) {
      mp_digit y0 = b[j], y1 = b[j + L];
      b[j]     = s_mp_ntt_mont_add(mc, y0, y1);                    /* q = 0, W^0 = 1 */
      b[j + L] = s_mp_ntt_mont_mul(mc, s_mp_ntt_mont_sub(mc, y0, y1), w[e]);
      e += step;
      if (e >= m) {
         e -= m;
      }
   }
}
#endif


#endif
