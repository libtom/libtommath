#include "tommath_private.h"
#ifdef S_MP_NTT_MONT_INIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
void s_mp_ntt_mont_init(ntt_mont *m, mp_digit p)
{
   mp_digit inv, r;
   m->p = p;
   inv = s_mp_ntt_invmod_d(p);
   m->ninv = (mp_digit)(0 - inv);                     /* -p^{-1} mod 2^64 */
   r = (mp_digit)(((mp_word)1 << NTT_BASE_SIZE) % p); /* R mod p = 2^64 mod p */
   m->r2 = (mp_digit)((mp_word)r * r % p);            /* R^2 mod p */
}
#endif


#endif
