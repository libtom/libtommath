#include "tommath_private.h"
#ifdef BN_S_MP_FFT_MUL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* FFT Multiplication */

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

static mp_word s_mul_digit_sum(const mp_int *a, const mp_int *b)
{
   mp_word dsa, dsb;

   dsa = s_digit_sum(a, a->used, (mp_word)(0));
   dsa %= MP_MASK;
   dsb = s_digit_sum(b, b->used, (mp_word)(0));
   dsb %= MP_MASK;

   return ((dsa * dsb) % MP_MASK);
}

int s_mp_fft_mul(const mp_int *a, const mp_int *b, mp_int *c)
{
   double *fa, *fb;
   int length;
   int e = MP_OKAY;
   mp_int r;
   mp_word cy, dsm, dsp;
   fa = NULL;
   fb = NULL;


   if (mp_iszero(a) == MP_YES || mp_iszero(b) == MP_YES) {
      return MP_OKAY;
   }
   if (a->used == 1) {
      if (a->dp[0] == (mp_digit)(1)) {
         if ((e = mp_copy(b,c)) != MP_OKAY) {
            return e;
         }
      }
      return e;
   }
   if (b->used == 1) {
      if (b->dp[0] == (mp_digit)(1)) {
         if ((e = mp_copy(a,c)) != MP_OKAY) {
            return e;
         }
      }
      return e;
   }
   if ((e = mp_init(&r)) != MP_OKAY) {
      return e;
   }
   if ((e = s_mp_dp_to_fft(a, &fa, b, &fb, &length)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = s_mp_fft(fa, fb, (unsigned long)length)) != MP_OKAY) {
      goto LTM_ERR;
   }
   cy = (mp_word)(0);
   if ((e = s_mp_fft_to_dp(fb, &r, length, &cy)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /* Check result */
   dsm = s_mul_digit_sum(a, b);
   dsp = s_digit_sum(&r, r.used, cy);
   if (dsm != dsp) {
      e = MP_VAL;
   }

LTM_ERR:
   if (fa != NULL) {
      MP_FREE(fa,sizeof(double) * (size_t)(length));
   }
   if (fb != NULL) {
      MP_FREE(fb,sizeof(double) * (size_t)(length));
   }
   mp_exch(&r,c);
   mp_clear(&r);
   return e;
}


#endif
