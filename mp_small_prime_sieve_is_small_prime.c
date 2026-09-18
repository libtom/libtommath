#include "tommath_private.h"
#ifdef MP_SMALL_PRIME_SIEVE_IS_SMALL_PRIME_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* TODO: change name to  "mp_small_prime_sieve_is_prime" ("small" is already in it)
         leave the "sieve" part out completely in all of the public sieve functions?
         Yes, programmers and naming things is a mess.

         "It was, it is, and will always be, my dear firstborn son: Son_0"
*/
/*
 * Sets "result" to true if n is prime or false respectively.
 * Also sets "result" to zero in case of error.
 * Worst case runtime is: building a base sieve O(n log log n) and a segment
 * O(n log log n) and search the segment linearly O(n).
 */
mp_err mp_small_prime_sieve_is_small_prime(ERAT_UINT n, bool *result, mp_erat_sieve *sieve)
{
   mp_err err = MP_OKAY;
   ERAT_UINT a = 0, b = 0;

   if ((n < 2) || (n > ERAT_BIGGEST_PRIME)) {
      *result = false;
      return err;
   }

   if (n == 2) {
      *result = true;
      return err;
   }

   if ((n & 1) == 0) {
      *result = false;
      return err;
   }
   /* n is not 2 and odd, leaves 3,4,5,6,7 both composites are even, rest is prime.
      Found no difference in runtime, maybe even slightly slower.
      But all the large tests are sequential, so it maybe useful for random very small input?
    */
   if (n <= 7) {
      *result = 1;
      return err;
   }

   if (sieve->base.content == NULL) {
      if ((err = s_mp_erat_eratosthenes_init(ERAT_UINT_MAX_SQRT, &(sieve->base))) != MP_OKAY) {
         *result = false;
         return err;
      }
   }

   /* No need to generate a segment if n is in the base sieve */
   if (n < (ERAT_UINT_MAX_SQRT)) {
      /* might have been a small sieve, so check size of sieve first */
      if (n < sieve->base.size) {
         *result = (s_mp_mp_erat_sieve_get_bit(&(sieve->base), (n - 1) / 2) == 1)?true:false;
         return err;
      }
   }
   /* no further shortcuts to apply, build and search a segment */

   /* we have a segment and may be able to use it */
   if (sieve->segment.content != NULL) {
      a = sieve->single_segment_a;
      /* last segment may not fit into range_a_b */
      if (a > (ERAT_BIGGEST_PRIME - ERAT_UINT_MAX_SQRT)) {
         b = ERAT_BIGGEST_PRIME;
      } else {
         b = a + ERAT_UINT_MAX_SQRT;
      }
      /* check if n is inside the bounds of the segment */
      if (n >= a && n <= b) {
         *result = (s_mp_mp_erat_sieve_get_bit(&(sieve->segment), n - a))?true:false;
         return err;
      }
   }

   /*
    * A bit of heuristics ( "heuristics" is a more pretentious word for the
    * commonly known expression "wild guess")
    * Based on the vague idea of the assumption that most sieves get used for
    * sequential series of primes or a single test here and there, but not
    * for massive amounts of random requests.
    */
   if (n > a) {
      if (n > (ERAT_UINT_MAX - ERAT_UINT_MAX_SQRT)) {
         a = ERAT_UINT_MAX - ERAT_UINT_MAX_SQRT;
      } else {
         a = n;
      }
   } else {
      a = n - ERAT_UINT_MAX_SQRT;
   }
   if ((err = s_mp_erat_init_single_segment_with_start(a,
              &(sieve->base), &(sieve->segment), &(sieve->single_segment_a))) != MP_OKAY) {
      *result = 0;
      return err;
   }
   /* finally, check for primality */
   *result = (s_mp_mp_erat_sieve_get_bit(&(sieve->segment), n - a))?true:false;
   return err;
}



#endif
