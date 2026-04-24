#include "tommath_private.h"
#ifdef S_MP_ROOT_N_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* find the n'th root of an integer
 *
 * Result found such that (c)**b <= a and (c+1)**b > a
 *
 * This algorithm uses Newton's approximation
 * x[i+1] = x[i] - f(x[i])/f'(x[i])
 * which will find the root in log(N) time where
 * each step involves a fair bit.
 */

mp_err s_mp_root_n(const mp_int *a, int b, mp_int *c, bool *is_perfect_power)
{
   mp_int t1, t2, t3, a_;
   int    ilog2;
   mp_word fp_log;
   mp_err err = MP_OKAY;

   if (is_perfect_power != NULL) {
      *is_perfect_power = false;
   }

   if (b == 0) {
      mp_set(c, 0);
      return MP_VAL;
   }

   /* 0^(1/x) = 0 with x != 0 is allowed */
   if (mp_iszero(a)) {
      mp_set(c, 0);
      return MP_OKAY;
   }

   if (b < 0 || (unsigned)b > (unsigned)MP_DIGIT_MAX) {
      return MP_VAL;
   }

   /* input must be positive if b is even */
   if (((b & 1) == 0) && mp_isneg(a)) {
      return MP_VAL;
   }

   if ((err = mp_init_multi(&t1, &t2, &t3, NULL)) != MP_OKAY) {
      return err;
   }

   /* if a is negative fudge the sign but keep track */
   a_ = *a;
   a_.sign = MP_ZPOS;

   /* Compute seed: 2^(log_2(n)/b + 2)*/
   ilog2 = mp_count_bits(a);

   /*
     If "b" is larger than INT_MAX it is also larger than
     log_2(n) because the bit-length of the "n" is measured
     with an int and hence the root is always < 2 (two).
   */
   if (b > INT_MAX/2) {
      mp_set(c, 1uL);
      c->sign = a->sign;
      err = MP_OKAY;
      goto LBL_ERR;
   }

   /* "b" is smaller than INT_MAX, we can cast safely */
   if (ilog2 < b) {
      mp_set(c, 1uL);
      c->sign = a->sign;
      err = MP_OKAY;
      goto LBL_ERR;
   }

   ilog2 =  ilog2 / b;
   if (ilog2 == 0) {
      mp_set(c, 1uL);
      c->sign = a->sign;
      err = MP_OKAY;
      goto LBL_ERR;
   }
   /* log2(a), fixed point */
   if ((err = s_mp_fp_log_d(&a_, &fp_log)) != MP_OKAY)                                                    goto LBL_ERR;
   s_mp_set_word(&t2, fp_log);

   /* This algorithm is sensitive to start-values that are too low. */
   mp_set(&t1, 1);
   if ((err = mp_mul_2d(&t1,MP_PRECISION_FIXED_LOG - 1, &t1)) != MP_OKAY)                                 goto LBL_ERR;
   if ((err = mp_add(&t2, &t1, &t2)) != MP_OKAY)                                                          goto LBL_ERR;

   /* k = log2(a) / b, fixed point */
   mp_set_i64(&t1, b);
   if ((err = mp_mul_2d(&t1,MP_FP_SCALE_LOG, &t1)) != MP_OKAY)                                            goto LBL_ERR;
   if ((err = mp_mul_2d(&t2,MP_FP_SCALE_LOG, &t2)) != MP_OKAY)                                            goto LBL_ERR;
   if ((err = mp_div(&t2, &t1, &t2,NULL)) != MP_OKAY)                                                     goto LBL_ERR;

   /* 2^k, the start-value */
   if ((err = mp_mul_2d(&t2,MP_FP_SCALE_LOG, &t2)) != MP_OKAY)                                            goto LBL_ERR;
   if ((err = s_mp_fp_exp2(&t2, &t2)) != MP_OKAY)                                                         goto LBL_ERR;
   /* Add 1 (one) unconditionally for the small roots.  */
   if ((err = mp_incr(&t2)) != MP_OKAY)                                                                   goto LBL_ERR;
   do {
      /* t1 = t2 */
      if ((err = mp_copy(&t2, &t1)) != MP_OKAY)                                                           goto LBL_ERR;
      /* t2 = t1 - ((t1**b - a) / (b * t1**(b-1))) */
      /* t3 = t1**(b-1) */
      if ((err = mp_expt_n(&t1, b - 1, &t3)) != MP_OKAY)                                                  goto LBL_ERR;
      /* numerator */
      /* t2 = t1**b */
      if ((err = mp_mul(&t3, &t1, &t2)) != MP_OKAY)                                                       goto LBL_ERR;
      /* t2 = t1**b - a */
      if ((err = mp_sub(&t2, &a_, &t2)) != MP_OKAY)                                                       goto LBL_ERR;
      /* denominator */
      /* t3 = t1**(b-1) * b  */
      if ((err = mp_mul_d(&t3, (mp_digit)b, &t3)) != MP_OKAY)                                             goto LBL_ERR;
      /* t3 = (t1**b - a)/(b * t1**(b-1)) */
      if ((err = mp_div(&t2, &t3, &t3, NULL)) != MP_OKAY)                                                 goto LBL_ERR;
      if ((err = mp_sub(&t1, &t3, &t2)) != MP_OKAY)                                                       goto LBL_ERR;
      /*
          Number of rounds is at most log_2(root). If it is more it
          got stuck, so break out of the loop and do the rest manually.
       */
      if (ilog2-- == 0) {
         break;
      }
   }  while (mp_cmp(&t1, &t2) != MP_EQ);

   /* result can be off by a few so check */
   /* Loop beneath can overshoot by one if root we found is smaller than the actual root */
   for (;;) {
      mp_ord cmp;
      if ((err = mp_expt_n(&t1, b, &t2)) != MP_OKAY)                                                      goto LBL_ERR;
      cmp = mp_cmp(&t2, &a_);
      if (cmp == MP_EQ) {
         /* It is a perfect power, flag it */
         if (is_perfect_power != NULL) {
            *is_perfect_power = true;
         }
         goto LBL_SET;
      }
      if (cmp == MP_LT) {
         if ((err = mp_add_d(&t1, 1uL, &t1)) != MP_OKAY)                                                  goto LBL_ERR;
      } else {
         break;
      }
   }
   /* correct overshoot from above or from recurrence */
   for (;;) {

      if ((err = mp_expt_n(&t1, b, &t2)) != MP_OKAY)                                                      goto LBL_ERR;
      if (mp_cmp(&t2, &a_) == MP_GT) {
         if ((err = mp_sub_d(&t1, 1uL, &t1)) != MP_OKAY)                                                  goto LBL_ERR;
      } else {
         break;
      }
   }
LBL_SET:
   /* set the result */
   mp_exch(&t1, c);

   /* set the sign of the result */
   c->sign = a->sign;

LBL_ERR:
   mp_clear_multi(&t1, &t2, &t3, NULL);
   return err;
}

#endif

