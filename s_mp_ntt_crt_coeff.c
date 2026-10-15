#include "tommath_private.h"
#ifdef S_MP_NTT_CRT_COEFF_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/*
   Recover one coefficient (from its residue modulo every prime) as a short
   array of 60-bit limbs.

   minv[i][j] = p_i^{-1} * 2^64 mod p_j, for i < j.  Propagating the digit d_i to
   the higher residues divides by p_i, so the update is
       x_j <- (x_j - d_i) * p_i^{-1} mod p_j.
   (M_i^{-1} = (p_0 ... p_{i-1})^{-1}, not p_i^{-1}, appears only when the partial
   value rather than the digit is propagated.)  Keeping p_i^{-1} in Montgomery
   form makes that update one multiplication with no division.
 */
void s_mp_ntt_crt_coeff(ntt_crt_ctx *cx, const mp_digit *r, mp_digit *limbs)
{
   int t = cx->t, i, j, w;
   mp_digit *x = cx->x;
   mp_digit *d = cx->d;
   mp_digit di, pj, diff;
   mp_word carry, v;

   for (i = 0; i < t; i++) x[i] = r[i];            /* each < p_i */

   for (i = 0; i < t; i++) {
      di = x[i];                          /* mixed-radix digit */
      d[i] = di;
      for (j = i + 1; j < t; j++) {
         pj = cx->p[j];
         diff = (x[j] >= di) ? (x[j] - di) : (x[j] + pj - di);
         x[j] = s_mp_ntt_mont_mul(&cx->mc[j], diff, cx->minv[i][j]);
      }
   }

   /* Horner: c = d_0 + p_0 (d_1 + p_1 (d_2 + ... )) */
   for (w = 0; w < NTT_MAXW; w++) limbs[w] = 0;
   limbs[0] = d[t - 1];
   for (i = t - 2; i >= 0; i--) {
      carry = (mp_word)d[i];
      for (w = 0; w < NTT_MAXW; w++) {
         v = (mp_word)limbs[w] * cx->p[i] + carry;
         limbs[w] = (mp_digit)(v & NTT_MASK);
         carry = v >> NTT_LIMB_SIZE;
      }
   }
}
#endif


#endif
