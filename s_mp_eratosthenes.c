#include "tommath_private.h"
#ifdef S_MP_ERATOSTHENES_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/*
 * Simple Eratosthenes' sieve, starting at zero
 * Keeping odd primes only as the single optimization
 */
void s_mp_eratosthenes(mp_single_sieve *bst)
{
   mp_sieve_prime n, k, r, j;
   n = bst->size;
   r = s_mp_isqrt(n);
   s_mp_sieve_setall(bst);
   for (k = 1uL; k < ((r - 1uL) / 2uL); k += 1uL) {
      if (s_mp_sieve_get(bst, k)) {
         for (j = k * 2uL * (k + 1uL); j < (n - 1uL) / 2uL; j += 2uL * k + 1uL) {
            s_mp_sieve_clear(bst, j);
         }
      }
   }
}

#endif
