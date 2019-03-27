#include "tommath_private.h"
#ifdef BN_MP_PRIMORIAL_C
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



/* Compute a primorial, the product of the first "n" primes */
#ifdef LTM_USE_EXTRA_FUNCTIONS
int mp_primorial(const LTM_SIEVE_UINT n, mp_int *p)
{
   mp_sieve *base = NULL;
   mp_sieve *segment = NULL;

   mp_factors factors;

   LTM_SIEVE_UINT single_segment_a = 0;
   LTM_SIEVE_UINT k, ret;

   int e = MP_OKAY;

   if ((e = mp_factors_init(&factors)) != MP_OKAY) {
      return e;
   }

   for (k = 0, ret = 0; ret < (LTM_SIEVE_UINT)n; k = ret) {
      if ((e = mp_next_small_prime(k + 1, &ret, &base,
                                   &segment, &single_segment_a)) != MP_OKAY) {
         if (e == LTM_SIEVE_MAX_REACHED) {
            if ((e = mp_set_long(p,(unsigned long)ret)) != MP_OKAY) {
               goto LTM_ERR;
            }
            if ((e = mp_factors_add(p, &factors)) != MP_OKAY) {
               goto LTM_ERR;
            }
            break;
         }
         goto LTM_ERR;
      }
      if (ret <= (LTM_SIEVE_UINT)n) {
         if ((e = mp_set_long(p,(unsigned long)ret)) != MP_OKAY) {
            goto LTM_ERR;
         }
         if ((e = mp_factors_add(p, &factors)) != MP_OKAY) {
            goto LTM_ERR;
         }
      }
   }

   if ((e = mp_factors_product(&factors, p)) != MP_OKAY) {
      goto LTM_ERR;
   }

LTM_ERR:
   mp_sieve_clear(base);
   mp_sieve_clear(segment);
   mp_factors_clear(&factors);
   return e;
}
#endif
#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
