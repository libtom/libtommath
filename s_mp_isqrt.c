#include "tommath_private.h"
#ifdef S_MP_ISQRT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/*
 * Integer square root, hardware style
 * Wikipedia calls it "Digit-by-digit calculation"
 * https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Digit-by-digit_calculation
 * This is the base 2 method described at
 * https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_(base_2)
 */
mp_sieve_prime s_mp_isqrt(mp_sieve_prime n)
{
   mp_sieve_prime s, rem, root;
   if (n < 1uL) {
      return 0uL;
   }

   /* highest power of four <= n */
   s = (mp_sieve_prime)(1uLL << (MP_SIEVE_PRIME_NUM_BITS - 2));
   rem = n;
   root = 0uL;
   while (s > n) {
      s >>= 2uL;
   }
   while (s != 0uL) {
      if (rem >= (s | root)) {
         rem -= (s | root);
         root >>= 1uL;
         root |= s;
      } else {
         root >>= 1uL;
      }
      s >>= 2uL;
   }

   return root;
}


#endif
