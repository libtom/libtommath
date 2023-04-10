#include "tommath_private.h"
#ifdef MP_LOG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#define MP_LOG2_EXPT(a,b) ((mp_count_bits((a)) - 1) / mp_cnt_lsb((b)))

static mp_err s_approx_log_d(const mp_int *a, const mp_int *b, int *lb)
{
   mp_word La, Lb;
   mp_err err;

   /* Approximation of the individual logarithms with low precision */
   if ((err = s_mp_fp_log_d(a, &La)) != MP_OKAY)                                                     goto LTM_ERR;
   if ((err = s_mp_fp_log_d(b, &Lb)) != MP_OKAY)                                                     goto LTM_ERR;

   /* Approximation of log_b(a) with low precision. */
   *lb = (int)(((La - (Lb + 1)/2) / Lb) + 1);
   /* TODO: just floor it instead? Multiplication is cheaper than division. */
   /* *lb = (int)(la_word / lb_word); */
   err = MP_OKAY;
LTM_ERR:
   return err;
}

static mp_err s_approx_log(const mp_int *a, const mp_int *b, int *lb)
{
   mp_int La, Lb, t;
   mp_err err;

   if ((err = mp_init_multi(&La, &Lb, &t, NULL)) != MP_OKAY) {
      return err;
   }

   if ((err = s_mp_fp_log(a, &La)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = s_mp_fp_log(b, &Lb)) != MP_OKAY)                                                            goto LTM_ERR;

   if ((err = mp_add_d(&Lb, 1u, &t)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_div_2(&t, &t)) != MP_OKAY)                                                               goto LTM_ERR;
   if ((err = mp_sub(&La, &t, &t)) != MP_OKAY)                                                            goto LTM_ERR;
   if ((err = mp_div(&t, &Lb, &t, NULL)) != MP_OKAY)                                                      goto LTM_ERR;
   if ((err = mp_add_d(&t, 1u, &t)) != MP_OKAY)                                                           goto LTM_ERR;

   *lb = mp_get_i32(&t);
   err = MP_OKAY;
LTM_ERR:
   mp_clear_multi(&t, &Lb, &La, NULL);
   return err;
}

mp_err mp_log(const mp_int *a, const mp_int *b, int *lb)
{
   mp_int bn;
   int n, fla, flb;
   mp_err err;
   mp_ord cmp;

   if (mp_isneg(a) || mp_iszero(a) || (mp_cmp_d(b, 2u) == MP_LT)) {
      return MP_VAL;
   }

   if (MP_IS_POWER_OF_TWO(b)) {
      *lb = MP_LOG2_EXPT(a, b);
      return MP_OKAY;
   }

   /* floor(log_2(x)) for cut-off */
   fla = mp_count_bits(a) - 1;
   flb = mp_count_bits(b) - 1;

   cmp = mp_cmp(a, b);

   /* "a < b -> 0" and "(b == a) -> 1" */
   if ((cmp == MP_LT) || (cmp == MP_EQ)) {
      *lb = (cmp == MP_EQ);
      return MP_OKAY;
   }

   /* "a < b^2 -> 1" (bit-count is sufficient, doesn't need to be exact) */
   if (((2 * flb)-1) > fla) {
      *lb = 1;
      return MP_OKAY;
   }

   if (MP_HAS(S_MP_WORD_TOO_SMALL)) {
      err = s_approx_log(a, b, &n);
   } else {
      err = s_approx_log_d(a, b, &n);
   }
   if (err != MP_OKAY) {
      return err;
   }

   if ((err = mp_init(&bn)) != MP_OKAY) {
      return err;
   }

   /* Check result. Result is wrong by 2(two) at most. */
   if ((err = mp_expt_n(b, n, &bn)) != MP_OKAY) {
      /* If the approximation overshot we can give it another try */
      if (err == MP_OVF) {
         n--;
         /* But only one */
         if ((err = mp_expt_n(b, n, &bn)) != MP_OKAY)                                                     goto LTM_ERR;
      } else {
         goto LTM_ERR;
      }
   }

   cmp = mp_cmp(&bn, a);

   /* The rare case of a perfect power makes a perfect shortcut, too. */
   if (cmp == MP_EQ) {
      *lb = n;
      goto LTM_OUT;
   }

   /* We have to make at least one multiplication because it could still be a perfect power. */
   if (cmp == MP_LT) {
      do {
         /* Full big-integer operations are to be avoided if possible */
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
      /* Overshot, take it back. */
      if (cmp == MP_GT) {
         n--;
      }
      goto LTM_OUT;
   }

   /* But it can overestimate, too, for example if "a" is closely below some "b^k" */
   if (cmp == MP_GT) {
      do {
         if (b->used == 1) {
            /* These are cheaper exact divisions, but that function is not available in LTM */
            if ((err = mp_div_d(&bn, b->dp[0], &bn, NULL)) != MP_OKAY)                                    goto LTM_ERR;

         } else {
            if ((err = mp_div(&bn, b, &bn, NULL)) != MP_OKAY)                                             goto LTM_ERR;
         }
         n--;
      } while ((cmp = mp_cmp(&bn, a)) == MP_GT);
   }

LTM_OUT:
   *lb = n;
   err = MP_OKAY;
LTM_ERR:
   mp_clear(&bn);
   return err;
}

#endif
