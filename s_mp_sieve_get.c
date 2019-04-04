#include "tommath_private.h"
#ifdef S_MP_SIEVE_GET_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* Read bit */
mp_sieve_prime s_mp_sieve_get(mp_single_sieve *bst, mp_sieve_prime n)
{
   return (((*((bst)->content+(n/MP_SIEVE_PRIME_NUM_BITS))
             & (1llu<<(n % MP_SIEVE_PRIME_NUM_BITS))) != 0));
}


#endif
