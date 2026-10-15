#include "tommath_private.h"
#ifdef S_MP_NTT_FROM_MONT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
mp_digit s_mp_ntt_from_mont(const ntt_mont *m, mp_digit a)
{
   return s_mp_ntt_mont_redc(m, (mp_word)a);
}
#endif


#endif
