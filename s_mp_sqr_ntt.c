#include "tommath_private.h"
#ifdef S_MP_SQR_NTT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/* The main function to square one mp_int into another mp_int */
mp_err s_mp_sqr_ntt(const mp_int *a, mp_int *c)
{
   mp_err err = MP_OKAY;
   /* TODO: nprimes is three, defined in NTT_MAXP, rewrite accordingly */
   int ncoef, m, minl, bound_bits, nprimes, pi, i;
   ntt_prime_ent *ents = NULL;
   ntt_crt_ctx   *cx = NULL;
   mp_digit *primes = NULL, **residues = NULL, *ra = NULL;
   mp_int P, pmp;


   if (mp_iszero(a)) {
      return mp_sqr(a,c);
   }
   if (a->used < 8) {
      return mp_sqr(a, c);
   }

   ncoef = a->used + a->used - 1; /* coefficients that carry the product */
   if ((ncoef > (1 << NTT_QUARTER_COEFF)) || (ncoef < 0)) {
      return MP_VAL;
   }
   m = s_mp_ntt_smooth_length(ncoef); /* smallest 5-smooth transform length */
   if (m < ncoef) {
      return MP_VAL;
   }

   /* Convolution bound: each output coefficient < min(la,lb) * 2^120. */
   minl = a->used;
   bound_bits = (int)NTT_LARGE_SIZE + s_mp_ntt_ilog2_ceil(minl) + 1;
   if (((bound_bits + NTT_LIMB_SIZE_M_ONE) / NTT_LIMB_SIZE) > NTT_MAXW) {
      return MP_VAL;
   }

   ents = (ntt_prime_ent *)MP_MALLOC((size_t)NTT_MAXP * sizeof(ntt_prime_ent));
   if (ents == NULL) {
      return MP_MEM;
   }

   if ((err = mp_init_multi(&P, &pmp, NULL)) != MP_OKAY) {
      MP_FREE(ents, (size_t)NTT_MAXP * sizeof(ntt_prime_ent));
      return err;
   }
   /* TODO */
   mp_set(&P, 1);

   /* Find primes until their product > 2^bound_bits */
   nprimes = 0;
   while (mp_count_bits(&P) <= bound_bits) {
      if (nprimes >= NTT_MAXP) {
         err = MP_VAL;
         goto LTM_ERR;
      }
      if (!s_mp_ntt_find_smooth_prime(&ents[nprimes], m, nprimes, 0)) {
         err = MP_VAL;
         goto LTM_ERR;
      }
      mp_set(&pmp, ents[nprimes].p);
      if ((err = mp_mul(&P, &pmp, &P)) != MP_OKAY) goto LTM_ERR;
      nprimes++;
   }

   s_mp_ntt_sort_primes(ents, nprimes);

   ra = (mp_digit *)MP_MALLOC((size_t)m * sizeof(mp_digit));
   if (ra == NULL) {
      err = MP_MEM;
      goto LTM_ERR;
   }
   residues = (mp_digit **)MP_CALLOC((size_t)nprimes, sizeof(mp_digit *));
   if (residues == NULL) {
      err = MP_MEM;
      goto LTM_ERR;
   }
   primes = (mp_digit *)MP_MALLOC((size_t)nprimes * sizeof(mp_digit));
   if (primes == NULL) {
      err = MP_MEM;
      goto LTM_ERR;
   }


   /* convolve over each prime and keep the residues */
   for (pi = 0; pi < nprimes; pi++) {
      residues[pi] = (mp_digit *)MP_MALLOC((size_t)ncoef * sizeof(mp_digit));
      if (residues[pi] == NULL) {
         err = MP_MEM;
         goto LTM_ERR;
      }
      if ((err = s_mp_ntt_autoconvolve_single(a, &ents[pi], ra, residues[pi], ncoef)) != MP_OKAY)        goto LTM_ERR;
      primes[pi] = ents[pi].p;
   }

   /* CRT: residues -> binary, then one linear carry sweep */
   cx = (ntt_crt_ctx *)MP_MALLOC(sizeof(ntt_crt_ctx));
   if (cx == NULL) {
      err = MP_MEM;
      goto LTM_ERR;
   }
   s_mp_ntt_crt_init(cx, primes, nprimes, bound_bits);
   err = s_mp_ntt_crt_assemble(cx, residues, nprimes, ncoef, c);

LTM_ERR:
   mp_clear_multi(&P, &pmp, NULL);
   if (residues) {
      for (i = 0; i < nprimes; i++) {
         MP_FREE(residues[i], (size_t)ncoef * sizeof(mp_digit));
      }
      MP_FREE(residues, (size_t)nprimes * sizeof(mp_digit *));
   }
   MP_FREE(primes, (size_t)nprimes * sizeof(mp_digit));
   MP_FREE(ra, (size_t)m * sizeof(mp_digit));
   MP_FREE(cx, sizeof(ntt_crt_ctx));
   MP_FREE(ents, (size_t)NTT_MAXP * sizeof(ntt_prime_ent));
   return err;
}
#endif


#endif
