#include "tommath_private.h"
#ifdef S_MP_NTT_AUTOCONVOLVE_SINGLE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
mp_err s_mp_ntt_autoconvolve_single(const mp_int *a, const ntt_prime_ent *ent, mp_digit *ra, mp_digit *out, int ncoef)
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
   }
   for (i = 0; i < a->used; i++) {
      ra[i] = s_mp_ntt_to_mont(&mc, a->dp[i]);
   }

   s_mp_ntt_smooth_fwd(&mc, ra, m, &R);

   for (i = 0; i < m; i++) {
      ra[i] = s_mp_ntt_mont_mul(&mc, ra[i], ra[i]);
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
