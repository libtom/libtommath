#include "tommath_private.h"
#ifdef MP_DIV_D_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* single digit division (based on routine from MPI) */
mp_err mp_div_d(const mp_int *a, mp_digit b, mp_int *c, mp_digit *d)
{
   mp_int  q;
   mp_word w;
   mp_err err = MP_OKAY;
   int ix;

   /* cannot divide by zero */
   if (b == 0u) {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   /* quick outs */
   if ((b == 1u) || mp_iszero(a)) {
      if (d != NULL) {
         *d = 0;
      }
      if (c != NULL) {
         if ((err = mp_copy(a, c)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR);
      }
      return err;
   }

   /* power of two ? */
   if (MP_HAS(MP_DIV_2) && (b == 2u)) {
      if (d != NULL) {
         *d = mp_isodd(a) ? 1u : 0u;
      }
      if (c != NULL) {
         if ((err = mp_div_2(a, c)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR);
      }
      return err;
   }
   if (MP_HAS(MP_DIV_2D) && MP_IS_2EXPT(b)) {
      ix = 1;
      while ((ix < MP_DIGIT_BIT) && (b != (((mp_digit)1)<<ix))) {
         ix++;
      }
      if (d != NULL) {
         *d = a->dp[0] & (((mp_digit)1<<(mp_digit)ix) - 1uL);
      }
      if (c != NULL) {
         if ((err = mp_div_2d(a, ix, c, NULL)) != MP_OKAY)              MP_TRACE_ERROR(err, LTM_ERR);
      }
      return err;
   }

   /* three? */
   if (MP_HAS(S_MP_DIV_3) && (b == 3u)) {
      if ((err = s_mp_div_3(a, c, d)) != MP_OKAY)                       MP_TRACE_ERROR(err, LTM_ERR);
      return err;
   }

   /* no easy answer [c'est la vie].  Just division */
   if ((err = mp_init_size(&q, a->used)) != MP_OKAY)                     MP_TRACE_ERROR(err, LTM_ERR);

   q.used = a->used;
   q.sign = a->sign;
   w = 0;
   for (ix = a->used; ix --> 0;) {
      mp_digit t = 0;
      w = (w << (mp_word)MP_DIGIT_BIT) | (mp_word)a->dp[ix];
      if (w >= b) {
         t = (mp_digit)(w / b);
         w -= (mp_word)t * (mp_word)b;
      }
      q.dp[ix] = t;
   }

   if (d != NULL) {
      *d = (mp_digit)w;
   }

   if (c != NULL) {
      mp_clamp(&q);
      mp_exch(&q, c);
   }
   mp_clear(&q);

LTM_ERR:
   return err;
}

#endif
