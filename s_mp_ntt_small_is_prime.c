#include "tommath_private.h"
#ifdef S_MP_NTT_SMALL_IS_PRIME_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
bool s_mp_ntt_small_is_prime(mp_digit n)
{
   /* More, but all primes < 2^15 */
   /* const mp_digit bases[12] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}; */
   /* All primes < 2^31 */
   const mp_digit bases[7] = {2u, 325u, 9375u, 28178u, 450775u, 9780504u, 1795265022u};
   int i, j, r;
   int composite;
   mp_digit a, x, d, p;

   if (n < 2u) {
      return false;
   }
   if (n == 2u) {
      return true;
   }
   if ((n & 1) == 0) {
      return false;
   }
   /* Yes, my dear linter, these are magic numbers, get used to it */
   for (p = 3u; (p < 1000u) && (p * p <= n); p+= 2u) {
      if (n % p == 0) {
         return (n == p);
      }
   }

   d = n - 1;
   r = 0;
   while (!(d & 1)) {
      d >>= 1;
      r++;
   }
   for (i = 0; i < 7; i++) {
      a = bases[i] % n;
      if (a == 0) {
         continue;
      }
      x = s_mp_ntt_small_powmod(a, d, n);
      if ((x == 1) || (x == n - 1)) {
         continue;
      }
      composite = 1;
      for (j = 1; j < r; j++) {
         x = s_mp_ntt_small_mulmod(x, x, n);
         if (x == (n - 1)) {
            composite = 0;
            break;
         }
      }
      if (composite) {
         return false;
      }
   }
   return true;
}
#endif



#endif
