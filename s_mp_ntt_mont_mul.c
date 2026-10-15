#include "tommath_private.h"
#ifdef S_MP_NTT_MONT_MUL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/* Montgomery multiply: a,b in [0,p) -> a*b*R^{-1} mod p. */
mp_digit s_mp_ntt_mont_mul(const ntt_mont *m, mp_digit a, mp_digit b)
{
   return s_mp_ntt_mont_redc(m, (mp_word)a * b);
}
#endif


#endif
