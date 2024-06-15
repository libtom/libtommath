#include "tommath_private.h"
#ifdef S_MP_FASTER_TO_RADIX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

static int s_mp_compute_s(int t, int k)
{
   uint32_t r = 0u;
   int log2_intmax, log2_k;

   log2_k = (int)s_mp_floor_ilog2(k) + 1;
   log2_intmax = (int)s_mp_floor_ilog2(INT_MAX) + 1;

   /* Rough first check for overflow */
   if (t > (log2_intmax - log2_k)) {
      return 0;
   }

   r = 1u << t;
   r = r * (uint32_t)k;

   /* Final check for overflow */
   return (r > (MP_MAX_DIGIT_COUNT * MP_DIGIT_BIT)) ? 0 : (int)r;
}

static mp_err s_mp_to_radix_recursive(const mp_int *a, char **str, size_t *part_maxlen, size_t *part_written,
                                      int radix, int32_t k, int32_t t, bool pad, bool first, mp_int *P, mp_int *R)
{
   mp_int r, q, a1;
   mp_err err;
   int Beta;

   if (t < 0) {
      /* Print the string from the number given */
      if ((err = s_mp_slower_to_radix(a, str, part_maxlen, part_written, radix, pad)) != MP_OKAY)        goto LTM_ERR;
   } else {
      if ((err = mp_init_multi(&q, &r, &a1, NULL)) != MP_OKAY)                                           goto LTM_ERR;
      if (MP_IS_POWER_OF_TWO(&P[t])) {
         if ((err = mp_div_2d(a, mp_count_bits(&P[t]) - 1, &q, &r)) != MP_OKAY)                          goto LTM_ERR;
      } else if (first) {
         /* Largest division, only one time, no reason for Barret division in the first place */
         if ((err = mp_div(a, &P[t], &q, &r)) != MP_OKAY)                                                goto LTM_ERR;
         /* Release early and often, they say. */
         mp_clear(&P[t]);
         mp_clear(&R[t]);
      } else {
         /*
            Barrett reduction. A step by step proof can be found at
            https://www.nayuki.io/page/barrett-reduction-algorithm

            See also: Modern Computer Arithmetic, version 0.5.9, page 59
          */
         Beta = (int)s_mp_compute_s(t+1, k);
         if (Beta == 0) {
            err = MP_OVF;
            goto LTM_ERR;
         }
         /* Q = floor(A1 * I / 2^Beta) */
         /* I = floor( (2^(2*Beta)) / B) Here we have R[t] = I, P[t] = B */
         /* TODO: We don't need the full "a" only  the upper part: a = a_1\beta + a_0 with 0 < a_0 < \beta
                  The cutoff with s_mp_mul_high is so low that the gap between that and the general cutoff
                  is too small to be worth the hassle.
                  But if somebody implements Thom Mulder's short products...
                  (There are successors. See e.g. D. Harvey and P. Zimmermann "Short Division of Long Integers",
                   Laszlo Hars "Fast Truncated Multiplication for Cryptographic Applications", Daniel Lemire "Exact Short
                   Products From Truncated Multipliers", and many^Wsome more.
          */
         if ((err = mp_mul(a, &R[t], &q)) != MP_OKAY)                                                    goto LTM_ERR;
         if ((err = mp_div_2d(&q, Beta, &q, NULL)) != MP_OKAY)                                           goto LTM_ERR;

         /* R = A - Q*B */
         /* TODO: Q*B can be a low short-product */
         if ((err = mp_mul(&q, &P[t], &r)) != MP_OKAY)                                                   goto LTM_ERR;
         if ((err = mp_sub(a, &r, &r)) != MP_OKAY)                                                       goto LTM_ERR;

         /* We can use this simple correction because of the way we computed the reciprocal */
         if (r.sign == MP_NEG) {
            if ((err = mp_decr(&q)) != MP_OKAY)                                                          goto LTM_ERR;
            if ((err = mp_add(&r, &P[t], &r)) != MP_OKAY)                                                goto LTM_ERR;
         }
      }
      /* Go down the lists while climbing up the tree. */
      t--;

      if (mp_iszero(&q) && (!pad)) {
         if ((err = s_mp_to_radix_recursive(&r, str, part_maxlen, part_written, radix,
                                            k, t, false, false, P, R)) != MP_OKAY)                       goto LTM_ERR;
      } else {
         if ((err = s_mp_to_radix_recursive(&q, str, part_maxlen, part_written, radix,
                                            k, t,  pad, false, P, R)) != MP_OKAY)                        goto LTM_ERR;
         if ((err = s_mp_to_radix_recursive(&r, str, part_maxlen, part_written, radix,
                                            k, t, true, false, P, R)) != MP_OKAY)                        goto LTM_ERR;
      }
      mp_clear_multi(&q, &r, &a1, NULL);
   }

   err = MP_OKAY;
LTM_ERR:
   return err;
}

mp_err s_mp_faster_to_radix(const mp_int *a, char *str, size_t maxlen, size_t *written, int radix)
{
   mp_err err = MP_OKAY;
   int n = 0, k, t = 0, steps = 0, ilog2a, s;

   /* Use given buffer directly, no temporary buffers for the individual chunks */
   char **sptr = &str;
   /* Size of the chunk */
   size_t part_written = 0;
   size_t part_maxlen = maxlen;

   /* List of reciprocals */
   mp_int *R = NULL;
   /* List of moduli */
   mp_int *P = NULL;

   mp_int T;


   /* Denominator for the reciprocal: b^y. */
   if ((err = mp_init_set(&T, (mp_digit)radix)) != MP_OKAY) {
      return err;
   }
   if ((err = mp_expt_n(&T, (int)s_mp_radix_exponent_y[radix],  &T)) != MP_OKAY)                         goto LTM_ERR0;
   n = (int)T.dp[0];

   /* Numerator of the reciprocal: ceil(log_2(n)) */
   k = (int)s_mp_floor_ilog2(n) + 1;

   /* steps = floor(log_2(floor(log_2(a))))*/
   ilog2a = mp_count_bits(a) - 1;

   /* Cutoff at about twice the size of P[0]. */
   if (ilog2a < (2 * k * MP_RADIX_BARRETT_START_MULTIPLICATOR)) {
      if ((err = s_mp_slower_to_radix(a, sptr, &part_maxlen, &part_written, radix, false)) != MP_OKAY)   goto LTM_ERR;
      /* part_written does not count EOS */
      *written = part_written + 1;
      mp_clear(&T);
      return err;
   }
   /*
      floor(log_2(floor(log_2(a)))) is a bit too much (we do not start at zero)
      but we check for the end inside the loop and the list is just a list of pointers,
      not much memory wasted here if we allocate too much steps.
    */
   steps  = (int)s_mp_floor_ilog2(ilog2a) + 1;
   /* Allocate memory for list of reciprocals */
   R = (mp_int *) MP_MALLOC((size_t) steps * sizeof(mp_int));
   if (R == NULL) {
      return MP_MEM;
   }
   /* Allocate memory for list of moduli */
   P = (mp_int *) MP_MALLOC((size_t) steps * sizeof(mp_int));
   if (P == NULL) {
      MP_FREE_BUF(R, (size_t) steps * sizeof(mp_int));
      return MP_MEM;
   }

   /*
      The approximation for the reciprocal used in Barrett's method is
          R_t = ceil(2^((2^t)*k)/n^(2^t))
      with R_0 = (2^(2*k))/b^y and k = ceil(log_2(n)) as computed above.
    */

   /* To get the tree a bit flatter. Alternative: do it iteratively instead of recursively */
   k = k * MP_RADIX_BARRETT_START_MULTIPLICATOR;


   /* Compute initial reciprocal R[0] and expand it (R[0]^(2^k) */
   if ((err = mp_init_i32(&P[0], n)) != MP_OKAY)                                                         goto LTM_ERR;
   if ((err = mp_expt_n(&P[0], MP_RADIX_BARRETT_START_MULTIPLICATOR, &P[0])) != MP_OKAY)                 goto LTM_ERR;
   if ((err = mp_init(&R[0])) != MP_OKAY)                                                                goto LTM_ERR;
   if ((err = mp_2expt(&R[0], 2*k)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_div(&R[0], &P[0], &R[0], NULL)) != MP_OKAY)                                             goto LTM_ERR;
   if ((err = mp_incr(&R[0])) != MP_OKAY)                                                                goto LTM_ERR;


   /* Compute the rest of the reciprocals as needed */
   for (t = 1; t < steps; t++) {
      /* P_t = (b^y)^(2^t) = n^(2^t) */
      /*
         We cannot just square because it can
            a) overflow MP_MAX_DIGIT_COUNT
            b) it can get bigger than "a" which it shouldn't
               which also means that
            c) if it gets bigger than "a" we have all necessary
               reciprocals and can break out of the loop
      */

      /* P[t-1]^2 > a is most likely more than just a bit or too, so check if we
         can bail out early without actually computing the square. */
      if ((2 * mp_count_bits(&P[t-1])) > ilog2a) {
         /* Correct index */
         t--;
         break;
      }

      /* Compute denominator */
      if ((err = mp_init(&P[t])) != MP_OKAY)                                                             goto LTM_ERR;
      /* P[t] = P[t-1]^2 */
      if ((err = mp_sqr(&P[t-1], &P[t])) != MP_OKAY)                                                     goto LTM_ERR;
      /* Check if P[t]^2 > a */
      if (mp_cmp(&P[t],a) == MP_GT) {
         /* We don't need P[t] anymore */
         mp_clear(&P[t]);
         /* Correct index */
         t--;
         break;
      }

      /* Compute numerator */
      if ((err = mp_init(&R[t])) != MP_OKAY)                                                            goto LTM_ERR;
      s = s_mp_compute_s(t + 1, k);
      /* Overflow, we have enough divisors */
      if (s == 0) {
         break;
      }
      if ((err = mp_2expt(&(R[t]), s)) != MP_OKAY)                                                    goto LTM_ERR;
      /* Compute reciprocal */
      /* R[t] = floor(2^(2^t * k) / P[t] */
      if (MP_IS_POWER_OF_TWO(&P[t])) {
         if ((err = mp_div_2d(&R[t], mp_count_bits(&P[t]) - 1, &R[t], NULL)) != MP_OKAY)              goto LTM_ERR;
      } else {
         if ((radix == 10) && ((2 * mp_count_bits(&P[t])) > ilog2a)) {
            break;
         }
         if ((err = mp_div(&R[t], &P[t], &R[t], NULL)) != MP_OKAY)                                    goto LTM_ERR;
      }
      if ((err = mp_incr(&R[t])) != MP_OKAY)                                                          goto LTM_ERR;
   }

   /* And finally: start the recursion. */
   if ((err = s_mp_to_radix_recursive(a, sptr, &part_maxlen, &part_written, radix,
                                      k, t, false, true, P, R)) != MP_OKAY)                              goto LTM_ERR;
   /* part_written does not account for EOS */
   *written = part_written + 1;

LTM_ERR:
   do {
      mp_clear(&P[t]);
      mp_clear(&R[t]);
   } while (t--);
   MP_FREE_BUF(P, (size_t) steps * sizeof(mp_int));
   MP_FREE_BUF(R, (size_t) steps * sizeof(mp_int));
LTM_ERR0:
   mp_clear(&T);
   return err;
}


#endif
