#include "tommath_private.h"
#ifdef S_MP_NTT_FWD5_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
void s_mp_ntt_fwd5(const ntt_mont *mc, mp_digit *b, int L, int step, const ntt_sroots *R)
{
   const mp_digit *w = R->w;
   int m = R->m, j;
   int s1 = step, s2 = 2 * step, s3 = 3 * step, s4 = 4 * step;   /* all < m */
   int e1 = 0, e2 = 0, e3 = 0, e4 = 0;
   mp_digit k0 = R->k5[0], k1 = R->k5[1], k2 = R->k5[2], k3 = R->k5[3];
   mp_digit y0, y1, y2, y3, y4, t1, t2, u1, u2, t1c1, t2c2, t1c2, t2c1, u1s1, u2s12, u1s12, u2s1, z1, z2, z3, z4;

   for (j = 0; j < L; j++) {
      y0 = b[j];
      y1 = b[j + L];
      y2 = b[j + 2 * L];
      y3 = b[j + 3 * L];
      y4 = b[j + 4 * L];
      t1 = s_mp_ntt_mont_add(mc, y1, y4);
      u1 = s_mp_ntt_mont_sub(mc, y1, y4);
      t2 = s_mp_ntt_mont_add(mc, y2, y3);
      u2 = s_mp_ntt_mont_sub(mc, y2, y3);
      t1c1 = s_mp_ntt_mont_mul(mc, t1, k0);
      u1s1  = s_mp_ntt_mont_mul(mc, u1, k2);
      t2c2 = s_mp_ntt_mont_mul(mc, t2, k1);
      u2s12 = s_mp_ntt_mont_mul(mc, u2, k3);
      t1c2 = s_mp_ntt_mont_mul(mc, t1, k1);
      u1s12 = s_mp_ntt_mont_mul(mc, u1, k3);
      t2c1 = s_mp_ntt_mont_mul(mc, t2, k0);
      u2s1  = s_mp_ntt_mont_mul(mc, u2, k2);
      z1 = s_mp_ntt_mont_add(mc, s_mp_ntt_mont_add(mc, y0, s_mp_ntt_mont_add(mc, t1c1, u1s1)),  s_mp_ntt_mont_add(mc, t2c2,
                             u2s12));
      z2 = s_mp_ntt_mont_add(mc, s_mp_ntt_mont_add(mc, y0, s_mp_ntt_mont_add(mc, t1c2, u1s12)), s_mp_ntt_mont_sub(mc, t2c1,
                             u2s1));
      z3 = s_mp_ntt_mont_add(mc, s_mp_ntt_mont_add(mc, y0, s_mp_ntt_mont_sub(mc, t1c2, u1s12)), s_mp_ntt_mont_add(mc, t2c1,
                             u2s1));
      z4 = s_mp_ntt_mont_add(mc, s_mp_ntt_mont_add(mc, y0, s_mp_ntt_mont_sub(mc, t1c1, u1s1)),  s_mp_ntt_mont_sub(mc, t2c2,
                             u2s12));

      b[j]         = s_mp_ntt_mont_add(mc, s_mp_ntt_mont_add(mc, y0, t1), t2);
      b[j + L]     = s_mp_ntt_mont_mul(mc, z1, w[e1]);
      b[j + 2 * L] = s_mp_ntt_mont_mul(mc, z2, w[e2]);
      b[j + 3 * L] = s_mp_ntt_mont_mul(mc, z3, w[e3]);
      b[j + 4 * L] = s_mp_ntt_mont_mul(mc, z4, w[e4]);
      e1 += s1;
      if (e1 >= m) {
         e1 -= m;
      }
      e2 += s2;
      if (e2 >= m) {
         e2 -= m;
      }
      e3 += s3;
      if (e3 >= m) {
         e3 -= m;
      }
      e4 += s4;
      if (e4 >= m) {
         e4 -= m;
      }
   }
}
#endif



#endif
