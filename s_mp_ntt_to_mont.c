#include "tommath_private.h"
#ifdef S_MP_NTT_TO_MONT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/* Convert x in [0,p) into Montgomery form (x * R mod p), and back. */
mp_digit s_mp_ntt_to_mont(const ntt_mont *m, mp_digit x)
{
   return s_mp_ntt_mont_mul(m, x, m->r2);
}
#endif
#endif

