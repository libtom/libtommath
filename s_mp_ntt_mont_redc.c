#include "tommath_private.h"
#ifdef S_MP_NTT_MONT_REDC_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */




/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/*  We rely on p < 2^63, which the NTT primes (< 2^62) always satisfy:
     - REDC's output r is in [0, 2p) and every input a, b, s is in [0, p);
     - subtracting p from a value below p wraps to a value whose top bit is
       set, because 2^64 - p > 2^63.  The borrow is therefore just bit 63 of
       the wrapped difference, and "add p back if we borrowed" is one mask and
       one add.  The same expression would NOT be valid for a modulus close to
       2^64 (e.g. 2^64 - 2^32 + 1): there 2^64 - p is small and the wrapped
       difference does not reach bit 63.
 */
/* REDC: T in [0, p*R) -> T * R^{-1} mod p, in [0, p). */
mp_digit s_mp_ntt_mont_redc(const ntt_mont *m, mp_word T)
{
   mp_digit q, r, d;
   mp_word u;
   q = (mp_digit)T * m->ninv;               /* (T mod R) * (-p^{-1}) mod R */
   u = T + (mp_word)q * m->p;               /* u < 2*p*R < 2^127 */
   r = (mp_digit)(u >> NTT_BASE_SIZE);      /* r < 2*p */
   d = r - m->p;
   d = d + (m->p & ((mp_digit)0u - (d >> NTT_BASE_SIZE_M_ONE)));
   return d;
}
#endif



#endif
