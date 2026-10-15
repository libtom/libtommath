#include "tommath_private.h"
#ifdef S_MP_NTT_CONVOLVE_SINGLE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/*
  One prime's contribution: forward-transform both inputs, multiply pointwise,
  inverse-transform, and return the convolution coefficients modulo that prime.

  The transform runs at m = ent->m (5-smooth, >= la + lb - 1), so its cyclic
  convolution of length m agrees with the linear one on the first
  ncoef = la + lb - 1 coefficients and is zero beyond - which is why only those
  are read back.
 */
mp_err s_mp_ntt_convolve_single(const mp_int *a, const mp_int *b, const ntt_prime_ent *ent, mp_digit *ra,
                                mp_digit *rb, mp_digit *out, int ncoef)
{
   int m = ent->m, i;
   ntt_mont mc;
   ntt_sroots R;
   mp_digit zeta;
   mp_err err = MP_OKAY;

   s_mp_ntt_mont_init(&mc, ent->p);
   zeta = s_mp_ntt_to_mont(&mc, ent->root);        /* primitive m-th root (Montgomery) */

   if ((err = s_mp_ntt_sroots_init(&R, &mc, m, zeta)) != MP_OKAY) return err;

   for (i = 0; i < m; i++) {
      ra[i] = 0;
      rb[i] = 0;
   }
   for (i = 0; i < a->used; i++) {
      ra[i] = s_mp_ntt_to_mont(&mc, a->dp[i]);
   }
   for (i = 0; i < b->used; i++) {
      rb[i] = s_mp_ntt_to_mont(&mc, b->dp[i]);
   }

   s_mp_ntt_smooth_fwd(&mc, ra, m, &R);
   s_mp_ntt_smooth_fwd(&mc, rb, m, &R);

   for (i = 0; i < m; i++) {
      ra[i] = s_mp_ntt_mont_mul(&mc, ra[i], rb[i]);
   }

   s_mp_ntt_smooth_inv(&mc, ra, m, &R);            /* includes the 1/m */

   for (i = 0; i < ncoef; i++) {
      out[i] = s_mp_ntt_from_mont(&mc, ra[i]);
   }

   s_mp_ntt_sroots_free(&R);
   return err;
}
#endif


#endif
