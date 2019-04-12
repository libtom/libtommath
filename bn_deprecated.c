#include "tommath_private.h"
#ifdef BN_DEPRECATED_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
/* LibTomMath, multiple-precision integer library -- Tom St Denis */

/* SPDX-License-Identifier: Unlicense */
#include <tommath_private.h>
#ifdef BN_FAST_MP_INVMOD_C
int fast_mp_invmod(const mp_int *a, const mp_int *b, mp_int *c)
{
   return s_mp_invmod_fast(a, b, c);
}
#endif
#ifdef BN_FAST_MP_MONTGOMERY_REDUCE_C
int fast_mp_montgomery_reduce(mp_int *x, const mp_int *n, mp_digit rho)
{
   return s_mp_montgomery_reduce_fast(x, n, rho);
}
#endif
#ifdef BN_FAST_S_MP_MUL_DIGS_C
int fast_s_mp_mul_digs(const mp_int *a, const mp_int *b, mp_int *c, int digs)
{
   return s_mp_mul_digs_fast(a, b, c, digs);
}
#endif
#ifdef BN_FAST_S_MP_MUL_HIGH_DIGS_C
int fast_s_mp_mul_high_digs(const mp_int *a, const mp_int *b, mp_int *c, int digs)
{
   return s_mp_mul_high_digs_fast(a, b, c, digs);
}
#endif
#ifdef BN_FAST_S_MP_SQR_C
int fast_s_mp_sqr(const mp_int *a, mp_int *b)
{
   return s_mp_sqr_fast(a, b);
}
#endif
#ifdef BN_MP_BALANCE_MUL_C
int mp_balance_mul(const mp_int *a, const mp_int *b, mp_int *c)
{
   return s_mp_balance_mul(a, b, c);
}
#endif
#ifdef BN_MP_EXPTMOD_FAST_C
int mp_exptmod_fast(const mp_int *G, const mp_int *X, const mp_int *P, mp_int *Y, int redmode)
{
   return s_mp_exptmod_fast(G, X, P, Y, redmode);
}
#endif
#ifdef BN_MP_INVMOD_SLOW_C
int mp_invmod_slow(const mp_int *a, const mp_int *b, mp_int *c)
{
   return s_mp_invmod_slow(a, b, c);
}
#endif
#ifdef BN_MP_KARATSUBA_MUL_C
int mp_karatsuba_mul(const mp_int *a, const mp_int *b, mp_int *c)
{
   return s_mp_karatsuba_mul(a, b, c);
}
#endif
#ifdef BN_MP_KARATSUBA_SQR_C
int mp_karatsuba_sqr(const mp_int *a, mp_int *b)
{
   return s_mp_karatsuba_sqr(a, b);
}
#endif
#ifdef BN_MP_TOOM_MUL_C
int mp_toom_mul(const mp_int *a, const mp_int *b, mp_int *c)
{
   return s_mp_toom_mul(a, b, c);
}
#endif
#ifdef BN_MP_TOOM_SQR_C
int mp_toom_sqr(const mp_int *a, mp_int *b)
{
   return s_mp_toom_sqr(a, b);
}
#endif
#ifdef BN_REVERSE_C
void bn_reverse(unsigned char *s, int len)
{
   s_mp_reverse(s, len);
}
#endif
#endif
