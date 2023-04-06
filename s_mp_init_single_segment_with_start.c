#include "tommath_private.h"
#ifdef S_MP_INIT_SINGLE_SEGMENT_WITH_START_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/*
 * Build a segment sieve with the largest reasonable size. "a" is the start of
 * the sieve Size is MIN(range_a_b,MP_SIEVE_PRIME_MAX-a)
 */
mp_err s_mp_init_single_segment_with_start(mp_sieve_prime a,
      mp_single_sieve *base_sieve,
      mp_single_sieve *single_segment,
      mp_sieve_prime *single_segment_a)
{
   mp_sieve_prime b;
   mp_err err = MP_OKAY;

   /* last segment might not fit, depending on size of range_a_b */
   if (a > (MP_SIEVE_PRIME_MAX - MP_SIEVE_RANGE_A_B)) {
      b = (mp_sieve_prime)MP_SIEVE_PRIME_MAX - 1uL;
   } else {
      b = a + (mp_sieve_prime)MP_SIEVE_RANGE_A_B;
   }
   if ((err = s_mp_eratosthenes_segment_init(a, b, single_segment, base_sieve)) != MP_OKAY) {
      return err;
   }
   *single_segment_a = a;
   return err;
}


#endif
