#include "tommath_private.h"
#ifdef MP_LOG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#define MP_IS_POWER_OF_TWO(a)    (((mp_count_bits((a)) - 1) ==  mp_cnt_lsb((a))) )
#define MP_LOG_POWER_OF_TWO(a,b) ( (mp_count_bits((a)) - 1) / (mp_count_bits((b)) - 1) )

/*
  This functions rely on the size of mp_word being larger than INT_MAX and in case
  there is a really weird architecture we try to check for it. Not a 100% reliable
  test but it has a safe fallback.
 */
#if ( (UINT_MAX == UINT32_MAX) && ( MP_WORD_SIZE > 4 ) ) \
      ||\
    ( (UINT_MAX == UINT16_MAX) && ( MP_WORD_SIZE > 2 ) )
mp_err mp_log(const mp_int *a, const mp_int *b, int *ln)
{
   mp_int bn, La, Lb;
   int n, fla, flb;
   mp_word la_word, lb_word;

   mp_err err;
   mp_ord cmp;

   if (mp_isneg(a) || mp_iszero(a) || (mp_cmp_d(b, 2u) == MP_LT)) {
      return MP_VAL;
   }

   if (MP_IS_POWER_OF_TWO(b)) {
      *ln = MP_LOG_POWER_OF_TWO(a,b);
      return MP_OKAY;
   }

   /* floor(log_2(x)) for cut-off */
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

   if ((err = mp_init_multi(&La, &Lb, &bn, NULL)) != MP_OKAY) {
      return err;
   }


   if ((err = s_mp_fp_log(a, &la_word)) != MP_OKAY)                                       goto LTM_ERR;
   if ((err = s_mp_fp_log(b, &lb_word)) != MP_OKAY)                                       goto LTM_ERR;

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
   mp_clear_multi(&La, &Lb, &bn, NULL);
   return err;
}


#else
/* Bigint version. A bit slower but not _that_ much  */
mp_err mp_log(const mp_int *a, const mp_int *b, int *ln)
{
   mp_int bn, La, Lb, t;

   int n, fla, flb;

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

   if ((err = s_mp_fp_log(a, &La)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = s_mp_fp_log(b, &Lb)) != MP_OKAY)                                                          goto LTM_ERR;


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
