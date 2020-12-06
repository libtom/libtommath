#include "tommath_private.h"
#ifdef MP_LOG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#define MP_IS_POWER_OF_TWO(a)    (((mp_count_bits((a)) - 1) ==  mp_cnt_lsb((a))) )
#define MP_LOG_POWER_OF_TWO(a,b) ( (mp_count_bits((a)) - 1) / (mp_count_bits((b)) - 1) )

#define MP_UPPER_LIMIT_FIXED_LOG ( (int) ( (sizeof(mp_word) * CHAR_BIT)      - 1))
#define MP_PRECISION_FIXED_LOG   ( (int) (((sizeof(mp_word) * CHAR_BIT) / 2) - 1))

/*
  This functions rely on the size of mp_word being larger than INT_MAX and in case
  there is a really weird architecture we try to check for it. Not a 100% reliable
  test but it should work.
 */
#if ((2*MP_DIGIT_BIT) > INT_MAX)

static mp_word s_flog2_mp_word(mp_word value)
{
   mp_word r = 0u;
   while ((value >>= 1) != 0u) {
      r++;
   }
   return r;
}

static mp_err s_log2_fixed_mp_word(mp_word x, int p, mp_word *c)
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

mp_err mp_log(const mp_int *a, const mp_int *b, int *ln)
{
   mp_int B, bn, La, Lb, t;
   int n, fla, flb;
   mp_word la_word, lb_word, tmp;
   int prec = MP_PRECISION_FIXED_LOG;

   mp_err err;
   mp_ord cmp;

   if (mp_isneg(a) || mp_iszero(a) || (mp_cmp_d(b, 2u) == MP_LT)) {
      return MP_VAL;
   }

   if (MP_IS_POWER_OF_TWO(b)) {
      *ln = MP_LOG_POWER_OF_TWO(a,b);
      return MP_OKAY;
   }

   /* floor(log_2(x)) for the range reduction */
   fla = mp_count_bits(a) - 1;
   flb = mp_count_bits(b) - 1;

   cmp = mp_cmp(a, b);

   /* "a < b -> 0" and "(b == a) -> 1" */
   if ((cmp == MP_LT) || (cmp == MP_EQ)) {
      *ln = (cmp == MP_EQ);
      return MP_OKAY;
   }

   /* "a < b^2 -> 1" (bit-count is sufficient, doesn't need to be exact) */
   if (((2 * flb)-1) > fla) {
      *ln = 1;
      return MP_OKAY;
   }

   if ((err = mp_init_multi(&B, &La, &Lb, &bn, &t, NULL)) != MP_OKAY) {
      return err;
   }

   /* We don't use the whole number, just the most significant "prec" bits */
   if (fla > prec) {
      if ((err = mp_div_2d(a, fla - prec, &t, NULL)) != MP_OKAY)                                          goto LTM_ERR;
      tmp = mp_get_u64(&t);
      if ((err = s_log2_fixed_mp_word(tmp, prec, &la_word)) != MP_OKAY)                                   goto LTM_ERR;
      tmp = ((mp_word)(fla - prec))<<prec;
      la_word += tmp;
   } else {
      tmp = mp_get_u64(a);
      if ((err = s_log2_fixed_mp_word(tmp, prec, &la_word)) != MP_OKAY)                                   goto LTM_ERR;
   }

   if (flb > prec) {
      if ((err = mp_div_2d(b, flb - prec, &t, NULL)) != MP_OKAY)                                          goto LTM_ERR;
      tmp = mp_get_u64(&t);
      if ((err = s_log2_fixed_mp_word(tmp, prec, &lb_word)) != MP_OKAY)                                   goto LTM_ERR;
      tmp = ((mp_word)(flb - prec))<<prec;
      lb_word += tmp;
   } else {
      tmp = mp_get_u64(b);
      if ((err = s_log2_fixed_mp_word(tmp, prec, &lb_word)) != MP_OKAY)                                   goto LTM_ERR;
   }

   /* Approximation of log_2(a)/log_2(b). Round to nearest. */
   n = (int)(((la_word - (lb_word + 1)/2) / lb_word) + 1);
   /* n = (int)(la_word / lb_word); */

   if ((err = mp_expt_n(b, n, &bn)) != MP_OKAY) {
      if (err == MP_OVF) {
         n--;
         /* One second chance only. */
         if ((err = mp_expt_n(b, n, &bn)) != MP_OKAY)                                                     goto LTM_ERR;
      } else {
         goto LTM_ERR;
      }
   }

   cmp = mp_cmp(&bn, a);

   /* The rare case of a perfect power makes a perfect shortcut, too. */
   if (cmp == MP_EQ) {
      *ln = n;
      goto LTM_OUT;
   }

   /* We have to make the one multiplication because it could still be a perfect power. */
   if (cmp == MP_LT) {
      do {
         if (b->used == 1) {
            if ((err = mp_mul_d(&bn, b->dp[0], &bn)) != MP_OKAY) {
               if (err == MP_OVF) {
                  goto LTM_OUT;
               }
               goto LTM_ERR;
            }
         } else {
            if ((err = mp_mul(&bn, b, &bn)) != MP_OKAY) {
               if (err == MP_OVF) {
                  goto LTM_OUT;
               }
               goto LTM_ERR;
            }
         }
         n++;
      } while ((cmp = mp_cmp(&bn, a)) == MP_LT);
      if (cmp == MP_GT) {
         n--;
      }
      goto LTM_OUT;
   }

   /* But it can overestimate, too, for example if "a" is closely below some "b^k" */
   if (cmp == MP_GT) {
      n--;
   }

LTM_OUT:
   *ln = n;
   err = MP_OKAY;
LTM_ERR:
   mp_clear_multi(&B, &La, &Lb, &bn, &t, NULL);
   return err;
}


#else

/* Bigint version. A bit slower but not _that_ much  */

static mp_err s_mp_fp_log_2(const mp_int *a, int p, mp_int *c)
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
mp_err mp_log(const mp_int *a, const mp_int *b, int *ln)
{
   mp_int bn, La, Lb, t;

   int n, fla, flb;
   /*
      We have arbitrary precision here and could adapt "prec" to actual precision,
      there is no need for full precision for small numbers. Some quick runtime
      tests revealed a gain that can only be called marginally at best.
      But: YMMV, as always.
    */
   int prec = MP_PRECISION_FIXED_LOG;

   mp_err err;
   mp_ord cmp;

   if (mp_isneg(a) || mp_iszero(a) || (mp_cmp_d(b, 2u) == MP_LT)) {
      return MP_VAL;
   }

   if (MP_IS_POWER_OF_TWO(b)) {
      *ln = MP_LOG_POWER_OF_TWO(a,b);
      return MP_OKAY;
   }

   fla = mp_count_bits(a) - 1;
   flb = mp_count_bits(b) - 1;

   cmp = mp_cmp(a, b);

   if ((cmp == MP_LT) || (cmp == MP_EQ)) {
      *ln = (cmp == MP_EQ);
      return MP_OKAY;
   }

   if ((2 * flb) > fla) {
      *ln = 1;
      return MP_OKAY;
   }

   if ((err = mp_init_multi(&La, &Lb, &bn, &t, NULL)) != MP_OKAY) {
      return err;
   }

   if (fla > prec) {
      if ((err = mp_div_2d(a, fla - prec, &t, NULL)) != MP_OKAY)                                          goto LTM_ERR;
      if ((err = s_mp_fp_log_2(&t, prec, &La)) != MP_OKAY)                                                goto LTM_ERR;
      mp_set_i32(&t,fla - prec);
      if ((err = mp_mul_2d(&t,prec, &t)) != MP_OKAY)                                                      goto LTM_ERR;
      if ((err = mp_add(&La, &t, &La)) != MP_OKAY)                                                        goto LTM_ERR;
   } else {
      if ((err = s_mp_fp_log_2(a, prec, &La)) != MP_OKAY)                                                 goto LTM_ERR;
   }

   if (flb > prec) {
      if ((err = mp_div_2d(b, flb - prec, &t, NULL)) != MP_OKAY)                                          goto LTM_ERR;
      if ((err = s_mp_fp_log_2(&t, prec, &Lb)) != MP_OKAY)                                                goto LTM_ERR;
      mp_set_i32(&t,flb - prec);
      if ((err = mp_mul_2d(&t,prec, &t)) != MP_OKAY)                                                      goto LTM_ERR;
      if ((err = mp_add(&Lb, &t, &Lb)) != MP_OKAY)                                                        goto LTM_ERR;
   } else {
      if ((err = s_mp_fp_log_2(b, prec, &Lb)) != MP_OKAY)                                                 goto LTM_ERR;
   }

   if ((err = mp_div_2(&Lb, &t)) != MP_OKAY)                                                             goto LTM_ERR;
   if ((err = mp_add(&La, &t, &La)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_div(&La, &Lb, &t, NULL)) != MP_OKAY)                                                    goto LTM_ERR;

   n = mp_get_i32(&t);

   if ((err = mp_expt_n(b, n, &bn)) != MP_OKAY) {
      if (err == MP_OVF) {
         n--;
         if ((err = mp_expt_n(b, n, &bn)) != MP_OKAY)                                                     goto LTM_ERR;
      } else {
         goto LTM_ERR;
      }
   }
   cmp = mp_cmp(&bn, a);

   if (cmp == MP_EQ) {
      *ln = n;
      goto LTM_OUT;
   }

   if (cmp == MP_LT) {
      do {
         if (b->used == 1) {
            if ((err = mp_mul_d(&bn, b->dp[0], &bn)) != MP_OKAY) {
               if (err == MP_OVF) {
                  goto LTM_OUT;
               }
               goto LTM_ERR;
            }
         } else {
            if ((err = mp_mul(&bn, b, &bn)) != MP_OKAY) {
               if (err == MP_OVF) {
                  goto LTM_OUT;
               }
               goto LTM_ERR;
            }
         }
         n++;
      } while ((cmp = mp_cmp(&bn, a)) == MP_LT);
      if (cmp == MP_GT) {
         n--;
      }
      goto LTM_OUT;
   }

   if (cmp == MP_GT) {
      n--;
   }

LTM_OUT:
   *ln = n;
   err = MP_OKAY;
LTM_ERR:
   mp_clear_multi(&La, &Lb, &bn, &t, NULL);
   return err;
}
#endif



#endif
