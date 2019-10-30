#include "tommath_private.h"
#ifdef MP_SQR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

static mp_err s_mp_sqr_noalias(const mp_int *a, mp_int *b)
{
   if (MP_HAS(S_MP_SQR_COMBA) && (a->used < (MP_MAX_COMBA / 2))) {
      return s_mp_sqr_comba(a, b);
   } else if (MP_HAS(S_MP_SQR)) {
      return s_mp_sqr(a, b);
   } else {
      return MP_VAL;
   }
}

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
   } else if (b->dp == a->dp) {
      mp_int t;
      if ((err = mp_init_size(&t, (2 * a->used) + 1)) != MP_OKAY) {
         return err;
      }
      if ((err = s_mp_sqr_noalias(a, &t)) != MP_OKAY) {
         mp_clear(&t);
         return err;
      }
      mp_exch(b, &t);
      mp_clear(&t);
   } else {
      if ((err = mp_grow(b, (2 * a->used) + 1)) != MP_OKAY) {
         return err;
      }
      mp_zero(b);
      err = s_mp_sqr_noalias(a, b);
   }
   b->sign = MP_ZPOS;
   return err;
}
#endif
