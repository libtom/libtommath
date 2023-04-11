#include "tommath_private.h"
#ifdef S_MP_FP_LOG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Fixed point bitwise logarithm base two of "a" with precision "p", Big-integer version. */
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
      there is no need for lowering precision for small numbers. Some quick runtime
      tests revealed a gain that can only be called marginally at best.
      But: YMMV, as always.
    */
   int prec = MP_PRECISION_FIXED_LOG;

   fla = mp_count_bits(a) - 1;

   if ((err = mp_init_multi(&La, &t, NULL)) != MP_OKAY) {
      return err;
   }

   if (fla > prec) {
      if ((err = mp_div_2d(a, fla - prec, &t, NULL)) != MP_OKAY)                                          goto LTM_ERR;
      if ((err = s_mp_fp_log_fraction(&t, prec,
                                      &La)) != MP_OKAY)                                                   goto LTM_ERR;
      mp_set_i32(&t,fla - prec);
      if ((err = mp_mul_2d(&t,prec, &t)) != MP_OKAY)                                                      goto LTM_ERR;
      if ((err = mp_add(&La, &t, &La)) != MP_OKAY)                                                        goto LTM_ERR;
   } else {
      if ((err = s_mp_fp_log_fraction(a, prec,
                                      &La)) != MP_OKAY)                                                   goto LTM_ERR;
   }

   mp_exch(&La, c);

LTM_ERR:
   mp_clear_multi(&La, &t, NULL);
   return err;
}


#endif
