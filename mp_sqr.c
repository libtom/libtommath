#include "tommath_private.h"
#ifdef MP_SQR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* computes b = a*a */
mp_err mp_sqr(const mp_int *a, mp_int *b)
{
   mp_err err;
   if (MP_HAS(S_MP_SQR_TOOM) && /* use Toom-Cook? */
       (a->used >= MP_SQR_TOOM_CUTOFF)) {
      err = s_mp_sqr_toom(a, b);
   } else if (MP_HAS(S_MP_SQR_KARATSUBA) &&  /* Karatsuba? */
              (a->used >= MP_SQR_KARATSUBA_CUTOFF)) {
      err = s_mp_sqr_karatsuba(a, b);
   } else if (MP_HAS(S_MP_SQR_COMBA) && /* can we use the fast comba multiplier? */
              (a->dp != b->dp) &&
              (a->used < (MP_MAX_COMBA / 2))) {
      err = s_mp_sqr_comba(a, b);
   } else if (MP_HAS(S_MP_SQR)) {
      err = s_mp_sqr(a, b);
   } else {
      err = MP_VAL;
   }
   b->sign = MP_ZPOS;
   return err;
}
#endif
