#include "tommath_private.h"
#ifdef S_MP_NTT_SROOTS_INIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/*
   Radix 5 needs more care than radix 2 or 3: the field has no imaginary part to
   split the transform into. With t = zeta^{m/5} and the pairs (y1,y4), (y2,y3),

        t1 = y1+y4,  u1 = y1-y4,  t2 = y2+y3,  u2 = y2-y3,
        c1 = t + t^{-1},  c2 = t^2 + t^{-2},
        s1 = t - t^{-1},  s12 = t^2 - t^{-2} = s1*c1,

   the five outputs are

        Z_0 = y0 + t1 + t2
        Z_1 = y0 + (t1*c1 + u1*s1 + t2*c2 + u2*s12)/2
        Z_4 = y0 + (t1*c1 - u1*s1 + t2*c2 - u2*s12)/2
        Z_2 = y0 + (t1*c2 + u1*s12 + t2*c1 - u2*s1)/2
        Z_3 = y0 + (t1*c2 - u1*s12 + t2*c1 + u2*s1)/2

   The 1/2 comes from writing each member of a pair through its sum and
   difference; folding it into the constants leaves the butterfly with eight
   multiplications per five elements - four distinct products per pair, each one
   reused by a sum and a difference. The inverse uses the same constants with
   s1 and s12 negated - those are the two that change sign under t -> t^{-1};
   c1 and c2 are symmetric.
 */
mp_err s_mp_ntt_sroots_init(ntt_sroots *R, ntt_mont *mc, int m, mp_digit zeta)
{
   int i, ell, n;

   R->m = m;
   R->nstages = 0;
   R->w = NULL;
   R->has3 = ((m % 3) == 0);
   R->has5 = ((m % 5) == 0);
   R->c3 = 0;
   R->c3i = 0;

   for (i = 0; i < 4; i++) {
      R->k5[i] = 0;
      R->k5i[i] = 0;
   }

   if (m < 2) {
      return MP_VAL;
   }
   n = s_mp_ntt_smooth_plan(m, R->radix, NTT_MR_MAXSTAGE);
   if (n <= 0) {
      return MP_VAL;
   }
   R->nstages = n;
   for (i = 0, ell = m; i < n; i++) {
      R->ell[i] = ell;
      ell /= R->radix[i];
   }

   R->w = (mp_digit *)MP_MALLOC((size_t)m * sizeof(mp_digit));
   if (R->w == NULL) {
      return MP_MEM;
   }
   R->w[0] = s_mp_ntt_to_mont(mc, 1);
   for (i = 1; i < m; i++) {
      R->w[i] = s_mp_ntt_mont_mul(mc, R->w[i - 1], zeta);
   }
   if (R->has3) {
      R->c3  = R->w[m / 3];            /* primitive cube root */
      R->c3i = R->w[m - m / 3];        /* its inverse */
   }
   if (R->has5) {
      mp_digit t   = R->w[m / 5];      /* primitive fifth root */
      mp_digit ti  = R->w[m - m / 5];  /* its inverse */
      mp_digit h   = s_mp_ntt_to_mont(mc, (mc->p + 1) >> 1);   /* 2^{-1} mod p */
      mp_digit c1  = s_mp_ntt_mont_add(mc, t, ti);
      /* TODO: is that readable or should distinct temporary variables be used? */
      mp_digit c2  = s_mp_ntt_mont_add(mc, s_mp_ntt_mont_mul(mc, t, t), s_mp_ntt_mont_mul(mc, ti, ti));
      mp_digit s1  = s_mp_ntt_mont_sub(mc, t, ti);
      mp_digit s12 = s_mp_ntt_mont_mul(mc, s1, c1);

      R->k5[0] = s_mp_ntt_mont_mul(mc, c1,  h);
      R->k5[1] = s_mp_ntt_mont_mul(mc, c2,  h);
      R->k5[2] = s_mp_ntt_mont_mul(mc, s1,  h);
      R->k5[3] = s_mp_ntt_mont_mul(mc, s12, h);
      R->k5i[0] = R->k5[0];
      R->k5i[1] = R->k5[1];
      R->k5i[2] = s_mp_ntt_mont_sub(mc, 0, R->k5[2]);   /* -h*s1  */
      R->k5i[3] = s_mp_ntt_mont_sub(mc, 0, R->k5[3]);   /* -h*s12 */
   }
   return MP_OKAY;
}
#endif


#endif
