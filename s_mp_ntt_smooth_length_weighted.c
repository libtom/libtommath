#include "tommath_private.h"
#ifdef S_MP_NTT_SMOOTH_LENGTH_WEIGHTED_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/* Gives the different radix sizes a different weight according to butterfly runtimes */
int s_mp_ntt_smooth_length_weighted(int n_min, int w2, int w3, int w5)
{
   int64_t next_pow2 = 1;
   int exp_pow2 = 0;
   int a, b, c, p5, p5_p3;
   int64_t n, current_n, size_limit, current_cost, cost;

   while (next_pow2 < n_min) {
      next_pow2 *= 2;
      exp_pow2++;
   }
   n = next_pow2;
   a = exp_pow2;
   b = 0;
   c = 0;
   cost = next_pow2 * (exp_pow2 * w2);

   size_limit = next_pow2 * 2;

   p5 = 1;
   for (c = 0; p5 <= size_limit; c++, p5 *= 5) {
      p5_p3 = p5;
      for (b = 0; p5_p3 <= size_limit; b++, p5_p3 *= 3) {
         /* Find the smallest 'a' to cross n_min */
         a = 0;
         current_n = p5_p3;
         while (current_n < n_min) {
            current_n *= 2;
            a++;
         }
         /* Just brute force */
         while (current_n <= size_limit) {
            current_cost = current_n * (a * w2 + b * w3 + c * w5);
            if (current_cost < cost) {
               cost = current_cost;
               n = current_n;
            }
            /* Pick the smaller in case of equal cost */
            else if (current_cost == cost && current_n < n) {
               n = current_n;
            }
            current_n *= 2;
            a++;
         }
         /* Check for overflow */
         if (p5_p3 > (INT_MAX / 3)) {
            break;
         }
      }
      if (p5 > (INT_MAX / 5)) {
         break;
      }
   }
   return (int)n;
}
#endif


#endif
