#include "tommath_private.h"
#ifdef MP_PRIME_RABIN_MILLER_TRIALS_RSA_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

int mp_prime_rabin_miller_trials_rsa(int size)
{
   return mp_prime_rabin_miller_trials(size);
}
#endif
