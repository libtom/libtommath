#include "tommath_private.h"
#ifdef BN_MP_TRIAL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * SPDX-License-Identifier: Unlicense
 */

/* Trial division with primes up to limit "limit", remainder in "r" */
#ifdef LTM_USE_EXTRA_FUNCTIONS

int mp_trial(const mp_int *a, int limit, mp_factors *factors, mp_int *r)
{
   mp_sieve *base = NULL;
   mp_sieve *segment = NULL;

   LTM_SIEVE_UINT single_segment_a = 0;
   LTM_SIEVE_UINT k, ret;

   mp_int divisor, quotient, remainder;

   int e;

   if ((e = mp_init_multi(&remainder, &divisor, &quotient, NULL)) != MP_OKAY) {
      return e;
   }
   if ((e = mp_copy(a, r)) != MP_OKAY) {
      goto LTM_ERR;
   }

   for (k = 0, ret = 0; ret < (LTM_SIEVE_UINT)limit; k = ret) {
      if ((e = mp_next_small_prime(k + 1, &ret, &base,
                                   &segment, &single_segment_a)) != MP_OKAY) {
         if (e == LTM_SIEVE_MAX_REACHED) {
            if ((e = mp_set_long(&divisor,(unsigned long)ret)) != MP_OKAY) {
               goto LTM_ERR;
            }
            do {
               if ((e = mp_div(r, &divisor, &quotient, &remainder)) != MP_OKAY) {
                  goto LTM_ERR;
               }
               if (IS_ZERO(&remainder)) {
                  if ((e = mp_factors_add(&divisor, factors)) != MP_OKAY) {
                     goto LTM_ERR;
                  }
                  mp_exch(r, &quotient);
               }
            } while (IS_ZERO(&remainder));
            /* found all of them */
            if (LTM_IS_UNITY(r)) {
               break;
            }
            break;
         }
         goto LTM_ERR;
      }

      if (ret <= (LTM_SIEVE_UINT)limit) {
         if ((e = mp_set_long(&divisor,(unsigned long)ret)) != MP_OKAY) {
            goto LTM_ERR;
         }
         do {
            if ((e = mp_div(r, &divisor, &quotient, &remainder)) != MP_OKAY) {
               goto LTM_ERR;
            }
            if (IS_ZERO(&remainder)) {
               if ((e = mp_factors_add(&divisor, factors)) != MP_OKAY) {
                  goto LTM_ERR;
               }
               mp_exch(r, &quotient);
            }
         } while (IS_ZERO(&remainder));

         /* found all of them */
         if (LTM_IS_UNITY(r)) {
            break;
         }
      }
   }

LTM_ERR:
   mp_clear_multi(&remainder, &divisor, &quotient, NULL);
   mp_sieve_clear(segment);
   mp_sieve_clear(base);
   return e;
}
#endif

#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
