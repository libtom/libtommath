#include "tommath_private.h"
#ifdef S_MP_SIEVE_CLEAR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */




/* Clear bit */
void s_mp_sieve_clear(mp_single_sieve *bst, mp_sieve_prime n)
{
   ((*((bst)->content+(n/MP_SIEVE_PRIME_NUM_BITS))
     &= ~(1llu<<(n % MP_SIEVE_PRIME_NUM_BITS))));
}


#endif
