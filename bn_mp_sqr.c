#include "tommath_private.h"
#ifdef BN_MP_SQR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* computes b = a*a */
int mp_sqr(const mp_int *a, mp_int *b)
{
   int     res;

#ifdef BN_S_MP_TOOM_SQR_C
   /* use Toom-Cook? */
   if (a->used >= TOOM_SQR_CUTOFF) {
      res = s_mp_toom_sqr(a, b);
      /* Karatsuba? */
   } else
#endif
#ifdef BN_S_MP_KARATSUBA_SQR_C
      if (a->used >= KARATSUBA_SQR_CUTOFF) {
         res = s_mp_karatsuba_sqr(a, b);
      } else
#endif
      {
#ifdef BN_S_MP_SQR_FAST_C
         /* can we use the fast comba multiplier? */
         if ((((a->used * 2) + 1) < (int)MP_WARRAY) &&
             (a->used <
              (int)(1u << (((CHAR_BIT * sizeof(mp_word)) - (2u * (size_t)MP_DIGIT_BIT)) - 1u)))) {
            res = s_mp_sqr_fast(a, b);
         } else
#endif
         {
#ifdef BN_S_MP_SQR_C
            res = s_mp_sqr(a, b);
#else
            res = MP_VAL;
#endif
         }
      }
   b->sign = MP_ZPOS;
   return res;
}
#endif
