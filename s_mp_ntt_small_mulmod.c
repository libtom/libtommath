#include "tommath_private.h"
#ifdef S_MP_NTT_SMALL_MULMOD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/* Non-montgomery for bootstrapping */
mp_digit s_mp_ntt_small_mulmod(mp_digit a, mp_digit b, mp_digit m)
{
   return (mp_digit)((mp_word)a * b % m);
}
#endif



#endif
