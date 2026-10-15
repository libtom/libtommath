#include "tommath_private.h"
#ifdef S_MP_NTT_MONT_ADD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/* Montgomery sub/add of residues in [0,p). */
mp_digit s_mp_ntt_mont_add(const ntt_mont *m, mp_digit a, mp_digit b)
{
   mp_digit d = a + b - m->p;
   return d + (m->p & ((mp_digit)0 - (d >> NTT_BASE_SIZE_M_ONE)));
}
#endif


#endif
