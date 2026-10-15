#include "tommath_private.h"
#ifdef S_MP_NTT_CRT_INIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
void s_mp_ntt_crt_init(ntt_crt_ctx *cx, const mp_digit *primes, int t, int bound_bits)
{
   int i, j;

   cx->t = t;
   cx->width = (bound_bits + NTT_LIMB_SIZE_M_ONE) / NTT_LIMB_SIZE;
   if (cx->width < 1) cx->width = 1;

   for (i = 0; i < t; i++) {
      cx->p[i] = primes[i];
      s_mp_ntt_mont_init(&cx->mc[i], primes[i]);
   }
   for (i = 0; i < t; i++) {
      for (j = i + 1; j < t; j++) {
         mp_digit pj = cx->p[j];
         mp_digit inv = s_mp_ntt_small_powmod(cx->p[i] % pj, pj - 2, pj);  /* p_i^{-1} mod p_j */
         cx->minv[i][j] = s_mp_ntt_to_mont(&cx->mc[j], inv);
      }
   }
}
#endif



#endif
