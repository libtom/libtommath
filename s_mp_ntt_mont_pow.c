#include "tommath_private.h"
#ifdef S_MP_NTT_MONT_POW_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/*
    Montgomery-form modular exponentiation: base^exp mod p, base in Montgomery
    form, result in Montgomery form.
 */
mp_digit s_mp_ntt_mont_pow(const ntt_mont *m, mp_digit base, mp_digit exp)
{
   /* 1 in Montgomery form. No define because it is not a constant here */
   mp_digit result = s_mp_ntt_to_mont(m, 1);
   mp_digit b = base;
   while (exp) {
      if (exp & 1) result = s_mp_ntt_mont_mul(m, result, b);
      b = s_mp_ntt_mont_mul(m, b, b);
      exp >>= 1;
   }
   return result;
}
#endif


#endif
