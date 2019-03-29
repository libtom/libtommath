#include "tommath_private.h"
#ifdef BN_MP_SMALL_PRIME_ARRAY_C
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

/* Fill a factor-array with small primes between "start" to "end" */
#ifdef LTM_USE_EXTRA_FUNCTIONS
int mp_small_prime_array(LTM_SIEVE_UINT start, LTM_SIEVE_UINT end, mp_factors *factors)
{
   mp_sieve sieve;
   LTM_SIEVE_UINT k, ret;
   mp_int p;

   int e = MP_OKAY;

   if (start > end) {
      return MP_VAL;
   }
   if (start > LTM_SIEVE_BIGGEST_PRIME) {
      return MP_VAL;
   }
   /* TODO: return MP_VAL better? */
   if (end > LTM_SIEVE_BIGGEST_PRIME) {
      end = LTM_SIEVE_BIGGEST_PRIME;
   }

   if ((e = mp_init(&p)) != MP_OKAY) {
      return e;
   }

   mp_sieve_init(&sieve);

   if (start > 0) {
      start--;
   }
   for (k = start, ret = 0; ret < end; k = ret) {
      if ((e = mp_next_small_prime(k + 1, &ret, &sieve)) != MP_OKAY) {
         if (e == LTM_SIEVE_MAX_REACHED) {
            if ((e = mp_set_long(&p,(unsigned long)ret)) != MP_OKAY) {
               goto LTM_ERR;
            }
            if ((e = mp_factors_add(&p, factors)) != MP_OKAY) {
               goto LTM_ERR;
            }
            break;
         }
         goto LTM_END;
      }
      if (ret <= end) {
         if ((e = mp_set_long(&p,(unsigned long)ret)) != MP_OKAY) {
            goto LTM_ERR;
         }
         if ((e = mp_factors_add(&p, factors)) != MP_OKAY) {
            goto LTM_ERR;
         }

      }
   }

LTM_ERR:
LTM_END:
   mp_sieve_clear(&sieve);
   mp_clear(&p);
   return e;
}
#endif

#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
