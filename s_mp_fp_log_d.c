#include "tommath_private.h"
#ifdef S_MP_FP_LOG_D_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

static mp_word s_mp_flog2_mp_word_d(mp_word value)
{
   mp_word r = 0u;
   while ((value >>= 1) != 0u) {
      r++;
   }
   return r;
}

/* Fixed point bitwise logarithm base two of "x" with precision "p"  */
static mp_err s_mp_fp_log_fraction_d(mp_word x, int p, mp_word *c)
{
   mp_word b, L_out, L, a_bar, twoep;
   int i;

   L = s_mp_flog2_mp_word_d(x);

   if ((L + (mp_word)p) > MP_UPPER_LIMIT_FIXED_LOG) {
      return MP_VAL;
   }

   a_bar = ((mp_word)p < L) ? x << (L - (mp_word)p) : x << ((mp_word)p - L);
   b = (mp_word)(1u) << (p - 1);
   L_out = L << p;

   twoep = (mp_word)(1u) << (p + 1);

   for (i = 0; i < p; i++) {
      a_bar = (a_bar * a_bar) >> p;
      if (a_bar >= twoep) {
         a_bar >>= 1u;
         L_out += b;
      }
      b >>= 1u;
   }
   *c = L_out;
   return MP_OKAY;
}

/* Approximate the base two logarithm of "a" */
mp_err s_mp_fp_log_d(const mp_int *a, mp_word *c)
{
   mp_err err;
   int la;
   int prec = MP_PRECISION_FIXED_LOG;
   mp_word tmp, la_word;
   mp_int t;

   la = mp_count_bits(a) - 1;

   /* We don't use the whole number, just the most significant "prec" bits */
   if (la > prec) {
      if ((err = mp_init(&t)) != MP_OKAY)                                                                 goto LTM_ERR;
      /* Get enough msb-bits for the chosen precision */
      if ((err = mp_div_2d(a, la - prec, &t, NULL)) != MP_OKAY)                                           goto LTM_ERR;
      tmp = mp_get_u64(&t);
      /* Compute the low precision approximation for the fractional part */
      if ((err = s_mp_fp_log_fraction_d(tmp, prec, &la_word)) != MP_OKAY)                                   goto LTM_ERR;
      /* Compute the integer part and add it */
      tmp = ((mp_word)(la - prec))<<prec;
      la_word += tmp;
      mp_clear(&t);
   } else {
      tmp = mp_get_u64(a);
      if ((err = s_mp_fp_log_fraction_d(tmp, prec, &la_word)) != MP_OKAY) {
         return err;
      }
   }

   *c = la_word;

   return MP_OKAY;
LTM_ERR:
   mp_clear(&t);
   return err;
}

#endif
