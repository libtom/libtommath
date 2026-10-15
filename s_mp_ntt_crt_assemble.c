#include "tommath_private.h"
#ifdef S_MP_NTT_CRT_ASSEMBLE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/*
    Residues -> the product.  residues[i] holds the first ncoef convolution
    coefficients modulo primes[i]; the result is written into c.
 */
mp_err s_mp_ntt_crt_assemble(ntt_crt_ctx *cx, mp_digit **residues, int nprimes,
                             int ncoef, mp_int *c)
{
   mp_err err = MP_OKAY;
   mp_digit *r, *acc, limbs[NTT_MAXW];
   mp_digit carry;
   int k, i, w, n;
   mp_word v;

   r = (mp_digit *)MP_MALLOC((size_t)nprimes * sizeof(mp_digit));
   if (r == NULL) {
      return MP_MEM;
   }
   /* lazy accumulator: one 60-bit slot per limb position; a slot receives at
    * most one limb from each of the <= NTT_MAXW overlapping coefficients, so it
    * stays below NTT_MAXW * 2^60 and cannot overflow */
   acc = (mp_digit *)MP_CALLOC((size_t)ncoef + NTT_MAXW, sizeof(mp_digit));
   if (acc == NULL) {
      MP_FREE(r, (size_t)nprimes * sizeof(mp_digit));
      return MP_MEM;
   }

   for (k = 0; k < ncoef; k++) {
      for (i = 0; i < nprimes; i++) {
         r[i] = residues[i][k];
      }
      s_mp_ntt_crt_coeff(cx, r, limbs);
      for (w = 0; w < cx->width; w++) {
         acc[k + w] += limbs[w];
      }
   }

   n = ncoef + NTT_MAXW;
   if ((err = mp_grow(c, n)) != MP_OKAY)        goto LTM_ERR;
   carry = 0;
   for (k = 0; k < n; k++) {
      v = (mp_word)acc[k] + carry;
      c->dp[k] = (mp_digit)(v & NTT_MASK);
      carry = (mp_digit)(v >> NTT_LIMB_SIZE);
   }
   c->used = n;
   c->sign = MP_ZPOS;
   mp_clamp(c);

LTM_ERR:
   MP_FREE(r, (size_t)nprimes * sizeof(mp_digit));
   MP_FREE(acc, ((size_t)ncoef + NTT_MAXW) * sizeof(mp_digit));
   return err;
}
#endif


#endif
