#include "tommath_private.h"
#ifdef BN_MP_SQR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* computes b = a*a */
int mp_sqr(const mp_int *a, mp_int *b)
{
   int     res;

   if (MP_ENABLED(MP_TOOM_SQR) && /* use Toom-Cook? */
       a->used >= TOOM_SQR_CUTOFF) {
      res = mp_toom_sqr(a, b);
   } else if (MP_ENABLED(MP_KARATSUBA_SQR) &&  /* Karatsuba? */
              a->used >= KARATSUBA_SQR_CUTOFF) {
      res = mp_karatsuba_sqr(a, b);
   } else if (MP_ENABLED(FAST_S_MP_SQR) && /* can we use the fast comba multiplier? */
              (((a->used * 2) + 1) < (int)MP_WARRAY) &&
              (a->used <
               (int)(1u << (((CHAR_BIT * sizeof(mp_word)) - (2u * (size_t)DIGIT_BIT)) - 1u)))) {
      res = fast_s_mp_sqr(a, b);
   } else if (MP_ENABLED(S_MP_SQR)) {
      res = s_mp_sqr(a, b);
   } else {
      res = MP_VAL;
   }
   b->sign = MP_ZPOS;
   return res;
}
#endif
