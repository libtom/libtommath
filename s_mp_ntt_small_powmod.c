#include "tommath_private.h"
#ifdef S_MP_NTT_SMALL_POWMOD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
/* Non-montgomery for bootstrapping */
mp_digit s_mp_ntt_small_powmod(mp_digit b, mp_digit e, mp_digit m)
{
   mp_digit r = 1u;
   b %= m;
   while (e) {
      if (e & 1) {
         r = s_mp_ntt_small_mulmod(r, b, m);
      }
      b = s_mp_ntt_small_mulmod(b, b, m);
      e >>= 1;
   }
   return r;
}
#endif



#endif
