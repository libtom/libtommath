#include "tommath_private.h"
#ifdef S_MP_FASTER_TO_RADIX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Portable integer log of two with small footprint */
static int32_t s_floor_ilog2(int32_t value)
{
   int r = 0;
   while ((value /= 2) != 0) {
      r++;
   }
   return r;
}

/* Exponentiation with small footprint */
static int32_t s_pow(int32_t base, int32_t exponent)
{
   int32_t result = 1;
   while (exponent != 0) {
      if ((exponent % 2) == 1) {
         result *= base;
      }
      exponent /= 2;
      base *= base;
   }
   return result;
}

static mp_err s_mp_to_radix_recursive(const mp_int *a, char **str, size_t *part_maxlen, size_t *part_written,
                                      int radix, int32_t k, int32_t t, bool pad, mp_int *P, mp_int *R)
{

   mp_int r, q, a1;
   mp_err err;
   int Beta;

   if (t < 0) {
      /* Print the string from the number given*/
      if ((err = s_mp_slower_to_radix(a, str, part_maxlen, part_written, radix, pad)) != MP_OKAY)        goto LTM_ERR;

   }  else {
      if ((err = mp_init_multi(&q, &r, &a1, NULL)) != MP_OKAY)                                           goto LTM_ERR;
      /*
         Barrett reduction. A step by step proof can be found at
         https://www.nayuki.io/page/barrett-reduction-algorithm

         See also: Modern Computer Arithmetic, version 0.5.9, page 59
       */

      /* If this cast-feast looks familiar: it is the numerator from computing the reciprocal*/
      Beta = (int)((int32_t)((uint32_t)1 << (t+1)) * k);

      /* Q = floor(A1 * I / 2^Beta) */
      /* I = floor( (2^(2*Beta)) / B) Here we have R[t] = I, P[t] = B */
      if ((err = mp_mul(a, &R[t], &q)) != MP_OKAY)                                                       goto LTM_ERR;
      if ((err = mp_div_2d(&q, Beta, &q, NULL)) != MP_OKAY)                                              goto LTM_ERR;

      /* R = A - Q*B */
      if ((err = mp_mul(&q, &P[t], &r)) != MP_OKAY)                                                      goto LTM_ERR;
      if ((err = mp_sub(a, &r, &r)) != MP_OKAY)                                                          goto LTM_ERR;

      /* We can use this simple correction because of the way we computed the reciprocal */
      if (r.sign == MP_NEG) {
         if ((err = mp_decr(&q)) != MP_OKAY)                                                             goto LTM_ERR;
         if ((err = mp_add(&r, &P[t], &r)) != MP_OKAY)                                                   goto LTM_ERR;
      }

      /* Go down the lists while climbing up the tree. */
      t--;

      /* Follow branches */
      if (mp_iszero(&q) && (!pad)) {
         if ((err = s_mp_to_radix_recursive(&r, str, part_maxlen, part_written, radix,
                                            k, t, false, P, R)) != MP_OKAY)                              goto LTM_ERR;
      } else {
         if ((err = s_mp_to_radix_recursive(&q, str, part_maxlen, part_written, radix,
                                            k, t,  pad, P, R)) != MP_OKAY)                               goto LTM_ERR;
         if ((err = s_mp_to_radix_recursive(&r, str, part_maxlen, part_written, radix,
                                            k, t, true, P, R)) != MP_OKAY)                               goto LTM_ERR;
      }
      mp_clear_multi(&q, &r, &a1, NULL);
   }

   err = MP_OKAY;
LTM_ERR:
   return err;
}


mp_err s_mp_faster_to_radix(const mp_int *a, char *str, size_t maxlen, size_t *written, int radix)
{
   mp_err err;
   int32_t n = 0, k, t = 0, steps;
   int ilog2a;

   /* Use given buffer directly, no temporary buffers for the individual chunks */
   char **sptr = &str;
   /* Size of the chunk */
   size_t part_written = 0;
   size_t part_maxlen = maxlen;

   /* List of reciprocals */
   mp_int *R = NULL;
   /* List of moduli */
   mp_int *P = NULL;

   /* Denominator for the reciprocal: b^y */
   n = s_pow((int32_t)radix, (int32_t)s_mp_radix_exponent_y[radix]);

   /* Numerator of the reciprocal: ceil(log_2(n)) */
   k = s_floor_ilog2(n) + 1;

   /* steps = floor(log_2(floor(log_2(a))))*/
   ilog2a = mp_count_bits(a) - 1;

   /* Cutoff at about twice the size of P[0]. Interestingly far below Karatsuba cut-off. */
   if (ilog2a < (2 * k * MP_RADIX_BARRETT_START_MULTIPLICATOR)) {
      if ((err = s_mp_slower_to_radix(a, sptr, &part_maxlen, &part_written, radix, false)) != MP_OKAY)   goto LTM_ERR;
      /* part_written does not count EOS */
      *written = part_written + 1;
      return err;
   }
   /*
      floor(log_2(floor(log_2(a)))) is not enough but we check for
      the end inside the loop and the list is just a list of pointers,
      not much memory wasted here.
    */
   steps  = s_floor_ilog2((int32_t)ilog2a) + 2;

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
      The approximation to the reciprocal used in Barrett's method is
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

   /* Compute the rest of the reciprocals if as needed */
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
      /* Check for overflow of 2^((2^t)*k) i.e. bigger than 2^MP_MAX_DIGIT_COUNT */
      if (((int)(1u << t)*k) > MP_MAX_DIGIT_COUNT) {
         /* TODO: This can only happen near MP_MAX_DIGIT_COUNT and we can use
                  the reciprocal R[t-1] to do the division but R[t] != R[t-1]^2
                  so we cannot just divide by R[t-1] twice.
          */
         err = MP_OVF;
         goto LTM_ERR;
      }

      /* P[t-1]^2 > a at most likely more than just a bit or too, so check if we
         can bail out early without actually computing the square. The
         constant "10" is comprised of unity plus some angst-allowance */
      if ((2 * mp_count_bits(&P[t-1]) - 10) > ilog2a) {
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
      if ((err = mp_init(&R[t])) != MP_OKAY)                                                             goto LTM_ERR;

      /* R[t] = R[t] << (2^t * k) The factor cannot overflow, we checked that above */
      /* TODO: these are more castings than in the ER in Mayrhofen at New Year's Eve! */
      if ((err = mp_2expt(&(R[t]), (int)((int32_t)((uint32_t)1 << (t+1)) * k))) != MP_OKAY)              goto LTM_ERR;

      /* Compute reciprocal */
      /* R[t] = floor(2^(2^t * k) / P[t] */
      if ((err = mp_div(&R[t], &P[t], &R[t], NULL)) != MP_OKAY)                                          goto LTM_ERR;
      /* Ceiling if P[t] is not a power of two but it is not a problem if P[t] is a power of two. */
      if ((err = mp_incr(&R[t])) != MP_OKAY)                                                             goto LTM_ERR;
   }

   /* And finally: start the recursion. */
   if ((err = s_mp_to_radix_recursive(a, sptr, &part_maxlen, &part_written, radix,
                                      k, t, false, P, R)) != MP_OKAY)                                    goto LTM_ERR;
   /* part_written does not account for EOS */
   *written = part_written + 1;

   err = MP_OKAY;
LTM_ERR:
   do {
      mp_clear(&P[t]);
      mp_clear(&R[t]);
   } while (t--);
   MP_FREE_BUF(P, (size_t) steps * sizeof(mp_int));
   MP_FREE_BUF(R, (size_t) steps * sizeof(mp_int));
   return err;
}

#endif
