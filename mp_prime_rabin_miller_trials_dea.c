#include "tommath_private.h"
#ifdef MP_PRIME_RABIN_MILLER_TRIALS_DEA_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Compute number of Miller-Rabin rounds necessary for DSA primes
   if only Miller-Rabin tests were used. Use
      mp_prime_rabin_miller_trials(size)
   or
      mp_prime_rabin_miller_trials_rsa(size)
   if the M-R tests are followed by a Lucas test.
 */
int mp_prime_rabin_miller_trials_dea(int error)
{
   return ((error/2) + 1);
}

#endif
