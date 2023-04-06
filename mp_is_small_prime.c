#include "tommath_private.h"
#ifdef MP_IS_SMALL_PRIME_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/*
 * Sets "result" to true if n is prime or false respectively.
 * Also sets "result" to zero in case of error.
 * Worst case runtime is: building a base sieve and a segment and
 * search the segment
 */
/* TODO: add ability to keep segments? */
mp_err mp_is_small_prime(mp_sieve_prime n, bool *result, mp_sieve *sieve)
{
   mp_err err = MP_OKAY;
   mp_sieve_prime a = 0uL, b = 0uL;

   if (n < 2uL) {
      *result = false;
      return err;
   }

   if (n == 2uL) {
      *result = true;
      return err;
   }

   if ((n & 1uL) == 0uL) {
      *result = false;
      return err;
   }

   /* neither of 2^16-x, 2^32-x, or 2^64-x are prime for 0<=x<=4 */
   if (n >= (mp_sieve_prime)(MP_SIEVE_PRIME_MAX - 3u)) {
      *result = false;
      return err;
   }

   if (sieve->base.content == NULL) {
      if ((err = s_mp_eratosthenes_init((mp_sieve_prime)MP_SIEVE_BASE_SIEVE_SIZE, &(sieve->base))) != MP_OKAY) {
         *result = false;
         return err;
      }
   }

   /* No need to generate a segment if n is in the base sieve */
   if (n < MP_SIEVE_BASE_SIEVE_SIZE) {
      /* might be a small sieve, so check size of sieve first */
      if (n < sieve->base.size) {
         *result = (s_mp_sieve_get(&(sieve->base), (n - 1uL) / 2uL) == 1ull);
         return err;
      } else {
         return MP_VAL;
      }
   }

   /* no further shortcuts to apply, build and search a segment */

   /* we have a segment and may be able to use it */
   if (sieve->segment.content != NULL) {
      a = sieve->single_segment_a;
      /* last segment may not fit into range_a_b */
      if (a > (MP_SIEVE_PRIME_MAX - MP_SIEVE_RANGE_A_B)) {
         b = (mp_sieve_prime)(MP_SIEVE_PRIME_MAX);
      } else {
         b = a + MP_SIEVE_RANGE_A_B;
      }
      /* check if n is inside the bounds of the segment */
      if (n >= a && n <= b) {
         *result = (s_mp_sieve_get(&(sieve->segment), n - a) == 1ull);
         return err;
      } else {
         a = 0ul;
         b = 0ul;
      }
   }

   if (n > a) {
      if (n > (MP_SIEVE_PRIME_MAX - MP_SIEVE_RANGE_A_B)) {
         a = (mp_sieve_prime)(MP_SIEVE_PRIME_MAX - MP_SIEVE_RANGE_A_B);
         b = (mp_sieve_prime) MP_SIEVE_PRIME_MAX;
      } else {
         a = n;
         b = a + (mp_sieve_prime)MP_SIEVE_RANGE_A_B;
      }
   }
   if ((err = s_mp_init_single_segment_with_start(a,
              &(sieve->base), &(sieve->segment), &(sieve->single_segment_a))) != MP_OKAY) {
      *result = false;
      return err;
   }
   /* finally, check for primality */
   *result = (s_mp_sieve_get(&(sieve->segment), n - a) == 1ull);
   return err;
}
#endif
