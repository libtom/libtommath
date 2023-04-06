#include "tommath_private.h"
#ifdef S_MP_ERATOSTHENES_INIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/*
 * Initiate a sieve that stores the odd numbers only:
 * allocate memory, set actual size and allocated size and fill it
 */
mp_err s_mp_eratosthenes_init(mp_sieve_prime n, mp_single_sieve *bst)
{
   size_t bits_in_sp;

   bits_in_sp = ((((size_t)n  + MP_SIEVE_PRIME_NUM_BITS - 1u)/MP_SIEVE_PRIME_NUM_BITS)/2u);
   bst->content =  MP_MALLOC(bits_in_sp * sizeof(mp_sieve_prime) + sizeof(mp_sieve_prime));
   if (bst->content == NULL) {
      return MP_MEM;
   }
   bst->alloc = (mp_sieve_prime)((bits_in_sp) * sizeof(mp_sieve_prime) + sizeof(mp_sieve_prime));
   bst->size = n;
   s_mp_eratosthenes(bst);
   return MP_OKAY;
}


#endif
