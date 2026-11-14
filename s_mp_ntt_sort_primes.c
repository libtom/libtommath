#include "tommath_private.h"
#ifdef S_MP_NTT_SORT_PRIMES_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/* TODO: There are at most four primes, no extra sort function necessary, do it inline */
/*
    Insertion sort by prime value. Both drivers select their primes largest
    first and hand them to Garner ascending.
 */
void s_mp_ntt_sort_primes(ntt_prime_ent *ents, int n)
{
   int a, b;

   for (a = 1; a < n; a++) {
      ntt_prime_ent key = ents[a];
      b = a - 1;
      while (b >= 0 && ents[b].p > key.p) {
         ents[b + 1] = ents[b];
         b--;
      }
      ents[b + 1] = key;
   }
}
#endif


#endif
