#include "tommath_private.h"
#ifdef S_MP_NTT_FIND_SMOOTH_PRIME_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/*
    Search for the `index`-th largest usable prime of a 5-smooth transform length
    m, i.e. p = k*m + 1 < 2^62 (index = 0 is the largest), requiring p > min_p.

    Largest rather than smallest for the same reason as ntt_find_large_prime:
    three ~62-bit primes cover the whole convolution bound at every supported
    size, so the prime count never grows with the input.

    The root search differs.  For the truncated path a quadratic non-residue gives
    a 2^s-th root for free (Euler's criterion), but m has 3s and 5s in it and no
    single test covers those.  So: w = a^((p-1)/m) has order dividing m, and it is
    exactly m as soon as w^(m/r) != 1 for every prime r dividing m - because every
    proper divisor of m divides one of those m/r.
 */
bool s_mp_ntt_find_smooth_prime(ntt_prime_ent *ent, int m, int index, mp_digit min_p)
{
   mp_digit mm, kmax, root;
   int seen = 0;
   mp_digit k, a, p, w;

   if (m < 2) {
      return false;
   }
   mm   = (mp_digit)m;
   kmax = (((mp_digit)1 << NTT_PRIME_MAX_SIZE) - 1) / mm;

   for (k = kmax; k >= 1; k--) {
      p = k * mm + 1;
      if (p >= ((mp_digit)1 << NTT_PRIME_MAX_SIZE)) {
         continue;
      }
      if (!s_mp_ntt_small_is_prime(p)) {
         continue;
      }
      if (p <= min_p) {
         return false;  /* cannot cover the bound */
      }
      if (seen++ != index) {
         continue;  /* want the index-th largest */
      }

      root = 0;
      /* Max. "a" measured was 48 with 60-bit */
      for (a = 2; (a < 1000) && (root == 0); a++) {
         w = s_mp_ntt_small_powmod(a, k, p);             /* order divides m */
         if (w == 1) {
            continue;
         }
         if (((m % 2) == 0) && s_mp_ntt_small_powmod(w, (mp_digit)m / 2, p) == 1) {
            continue;
         }
         if (((m % 3) == 0) && s_mp_ntt_small_powmod(w, (mp_digit)m / 3, p) == 1) {
            continue;
         }
         if (((m % 5) == 0) && s_mp_ntt_small_powmod(w, (mp_digit)m / 5, p) == 1) {
            continue;
         }
         root = w;
      }
      if (root == 0) {
         return false;  /* no root of full order */
      }
      ent->p = p;
      ent->root = root;
      ent->s = 0;
      ent->m = m;
      return true;
   }
   return false;
}
#endif


#endif
