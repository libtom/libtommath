#include "tommath_private.h"
#ifdef BN_S_MP_FFT_SQR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* Squaring with FFT */

/*
   FFT _will_ fail at one point, a small check is necessary.

   Idea shamelessly stolen from J. Arndt's book
   "Matters Computational" (ch. 28.4, p. 562) available for no cost at
   http://www.jjj.de/fxt/fxtbook.pdf
*/
static mp_word s_digit_sum(const mp_int *a, const int size, mp_word c)
{
   int i;
   for (i = 0; i < size; i++) {
      c += (mp_word)a->dp[i];
      c %= MP_MASK;
   }
   return c;
}

static mp_word s_sqr_digit_sum(const mp_int *a)
{
   mp_word dsa;

   dsa = s_digit_sum(a, a->used, (mp_word)(0));
   return ((dsa * dsa) % MP_MASK);
}

int s_mp_fft_sqr(const mp_int *a, mp_int *c)
{
   double *fa;
   int length;
   int e = MP_OKAY;
   mp_word dsm, dsp, cy;
   mp_int r;
   fa = NULL;

   if (mp_iszero(a) == MP_YES) {
      return e;
   }
   if (a->used == 1) {
      if (a->dp[0] == 1) {
         if ((e = mp_set_int(c,1)) != MP_OKAY) {
            return e;
         }
      }
      return e;
   }
   if ((e = mp_init(&r)) != MP_OKAY) {
      return e;
   }
   if ((e = s_mp_dp_to_fft_single(a, &fa, &length)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = s_mp_fft_sqr_ex(fa, (unsigned long)length)) != MP_OKAY) {
      goto LTM_ERR;
   }
   cy = (mp_word)(0);
   if ((e = s_mp_fft_to_dp(fa, &r, length, &cy)) != MP_OKAY) {
      goto LTM_ERR;
   }
   /* Check result */
   dsm = s_sqr_digit_sum(a);
   dsp = s_digit_sum(&r, r.used, cy);
   if (dsm != dsp) {
      e = MP_VAL;
   }

LTM_ERR:
   if (fa != NULL) {
      MP_FREE(fa,sizeof(double) * (size_t)(length));
   }
   mp_exch(&r,c);
   mp_clear(&r);
   return e;
}


#endif
