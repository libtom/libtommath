#include "tommath_private.h"
#ifdef S_MP_FP_LOG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Fixed point binary logarithm with small precision (half the bitsize of a mp_digit)*/
#if ( (UINT_MAX == UINT32_MAX) && ( MP_WORD_SIZE > 4 ) ) \
      ||\
    ( (UINT_MAX == UINT16_MAX) && ( MP_WORD_SIZE > 2 ) )
static mp_err s_mp_fp_log_fraction(mp_word x, int p, mp_word *c)
{
   mp_word b, L_out, L, a_bar, twoep;
   int i;

   L = s_flog2_mp_word(x);

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

mp_err s_mp_fp_log(const mp_int *a, mp_word *c)
{
   mp_err err;
   int la;
   int prec = MP_PRECISION_FIXED_LOG;
   mp_word tmp, la_word;
   mp_int t;

   la = mp_count_bits(a) - 1;

   /* We don't use the whole number, just the most significant "prec" bits */
   if (la > prec) {
      if ((err = mp_init(&t)) != MP_OKAY)                                                               goto LTM_ERR;
      if ((err = mp_div_2d(a, la - prec, &t, NULL)) != MP_OKAY)                                          goto LTM_ERR;
      tmp = mp_get_u64(&t);
      if ((err = s_mp_fp_log_fraction(tmp, prec, &la_word)) != MP_OKAY)                                  goto LTM_ERR;
      tmp = ((mp_word)(la - prec))<<prec;
      la_word += tmp;
      mp_clear(&t);
   } else {
      tmp = mp_get_u64(a);
      if ((err = s_mp_fp_log_fraction(tmp, prec, &la_word)) != MP_OKAY) {
         return err;
      }
   }

   *c = la_word;

   return MP_OKAY;
LTM_ERR:
   mp_clear(&t);
   return err;
}

#else
static mp_err s_mp_fp_log_fraction(const mp_int *a, int p, mp_int *c)
{
   mp_int b, L_out, twoep, a_bar;
   int L, pmL;
   int i;
   mp_err err;

   if ((err = mp_init_multi(&b, &L_out, &twoep, &a_bar, NULL)) != MP_OKAY) {
      return err;
   }

   L = mp_count_bits(a) - 1;
   pmL = (p < L) ? L - p: p - L;
   if ((err = mp_mul_2d(a, pmL, &a_bar)) != MP_OKAY)                                                      goto LTM_ERR;
   if ((err = mp_2expt(&b, p - 1)) != MP_OKAY)                                                            goto LTM_ERR;
   mp_set_i32(&L_out, L);
   if ((err = mp_mul_2d(&L_out, p, &L_out)) != MP_OKAY)                                                   goto LTM_ERR;

   if ((err = mp_2expt(&twoep, p + 1)) != MP_OKAY)                                                        goto LTM_ERR;

   for (i = 0; i < p; i++) {
      if ((err = mp_sqr(&a_bar, &a_bar)) != MP_OKAY)                                                      goto LTM_ERR;
      if ((err = mp_div_2d(&a_bar, p, &a_bar, NULL)) != MP_OKAY)                                          goto LTM_ERR;
      if (mp_cmp(&a_bar, &twoep) != MP_LT) {
         if ((err = mp_div_2(&a_bar, &a_bar)) != MP_OKAY)                                                 goto LTM_ERR;
         if ((err = mp_add(&L_out, &b, &L_out)) != MP_OKAY)                                               goto LTM_ERR;
      }
      if ((err = mp_div_2(&b, &b)) != MP_OKAY)                                                            goto LTM_ERR;
   }

   mp_exch(c, &L_out);

LTM_ERR:
   mp_clear_multi(&b, &L_out, &twoep, &a_bar, NULL);
   return err;
}

mp_err s_mp_fp_log(const mp_int *a, mp_int *c)
{
   mp_int La, t;
   mp_err err;
   int fla;
   /*
      We have arbitrary precision here and could adapt "prec" to actual precision,
      there is no need for full precision for small numbers. Some quick runtime
      tests revealed a gain that can only be called marginally at best.
      But: YMMV, as always.
    */
   int prec = MP_PRECISION_FIXED_LOG;

   fla = mp_count_bits(a) - 1;

   if ((err = mp_init_multi(&La, &t, NULL)) != MP_OKAY) {
      return err;
   }
   /* We don't use the whole number, just the most significant "prec" bits */
   if (fla > prec) {
      if ((err = mp_div_2d(a, fla - prec, &t, NULL)) != MP_OKAY)                                         goto LTM_ERR;
      if ((err = s_mp_fp_log_fraction(&t, prec,
                                      &La)) != MP_OKAY)                                                 goto LTM_ERR;
      mp_set_i32(&t,fla - prec);
      if ((err = mp_mul_2d(&t,prec, &t)) != MP_OKAY)                                                     goto LTM_ERR;
      if ((err = mp_add(&La, &t, &La)) != MP_OKAY)                                                       goto LTM_ERR;
   } else {
      if ((err = s_mp_fp_log_fraction(a, prec,
                                      &La)) != MP_OKAY)                                                  goto LTM_ERR;
   }

   mp_exch(&La, c);

LTM_ERR:
   mp_clear_multi(&La, &t, NULL);
   return err;
}

#endif
#endif
