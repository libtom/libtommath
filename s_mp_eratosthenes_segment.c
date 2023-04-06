#include "tommath_private.h"
#ifdef S_MP_ERATOSTHENES_SEGMENT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Fill sieve "segment" of the range [a,b] from the basic sieve "base" */
void s_mp_eratosthenes_segment(mp_sieve_prime a, mp_sieve_prime b,
                               mp_single_sieve *base, mp_single_sieve *segment)
{
   mp_sieve_prime r, j, i;
   mp_sieve_prime p;
   r = s_mp_isqrt(b);
   s_mp_sieve_setall(segment);
   p = 0u;
   for (i = 0u; p < r; i++) {
      if (p <= 1ul) {
         p = 2ul;
      } else {
         p = s_mp_sieve_nextset(base, ((p - 1uL) / 2uL) + 1uL);
         p = (2uL * p) + 1uL;
      }
      j = (a / p) * p;
      if (j < a) {
         j += p;
      }
      for (; j < b; j += p) {
         /* j+=p can overflow */
         if (j >= a) {
            s_mp_sieve_clear(segment, j - a);
         } else {
            break;
         }
      }
   }
}

#endif
