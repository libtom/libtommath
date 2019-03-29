#include "tommath_private.h"
#ifdef BN_MP_NEXT_SMALL_PRIME_C
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

/*
 * Mimics behaviour of Pari/GP's nextprime(n)
 * If n is prime set *result to n else set *result to first prime > n
 * and 0 in case of error
 */
#ifdef LTM_USE_EXTRA_FUNCTIONS
int mp_next_small_prime(LTM_SIEVE_UINT n, LTM_SIEVE_UINT *result, mp_sieve *sieve)
{
   LTM_SIEVE_UINT ret = 0;
   int e = MP_OKAY;

   if (n < 2) {
      *result = 2;
      return e;
   }

   for (; ret == 0 && n != 0; n++) {
      if (n > LTM_SIEVE_BIGGEST_PRIME) {
         return LTM_SIEVE_MAX_REACHED;
      }
      /* just call mp_is_small_prime(), it does all of the heavy work */
      if ((e = mp_is_small_prime(n, &ret, sieve)) != MP_OKAY) {
         *result = 0;
         return e;
      }
   }
   *result = n - 1;
   return e;
}
#endif


#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
