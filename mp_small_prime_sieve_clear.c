#include "tommath_private.h"
#ifdef MP_SMALL_PRIME_SIEVE_CLEAR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



static void s_mp_erat_clear_one(mp_erat_single_sieve *sieve)
{
   if (sieve->content != NULL) {
      MP_FREE(sieve->content, sieve->alloc);
      sieve->alloc = 0;
   }
   sieve->size = 0;
}

void mp_small_prime_sieve_clear(mp_erat_sieve *sieve)
{
   s_mp_erat_clear_one(&(sieve->base));
   s_mp_erat_clear_one(&(sieve->segment));
}


#endif
