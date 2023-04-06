#include "tommath_private.h"
#ifdef S_MP_SIEVE_NEXTSET_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* Searches next set bit independent of the kind of sieve */
mp_sieve_prime s_mp_sieve_nextset(mp_single_sieve *bst, mp_sieve_prime n)
{
   while ((n < ((bst)->size)) && (!s_mp_sieve_get(bst, n))) {
      n++;
   }
   return n;
}


#endif
