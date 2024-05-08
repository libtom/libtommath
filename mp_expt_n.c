#include "tommath_private.h"
#ifdef MP_EXPT_N_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* calculate c = a**b  using a square-multiply algorithm */
mp_err mp_expt_n(const mp_int *a, int b, mp_int *c)
{
   mp_err err = MP_OKAY;
   mp_int  g;

   if ((err = mp_init_copy(&g, a)) != MP_OKAY) {
      return err;
   }

   /* set initial result */
   mp_set(c, 1uL);

   while (b > 0) {
      /* if the bit is set multiply */
      if ((b & 1) != 0) {
         if ((err = mp_mul(c, &g, c)) != MP_OKAY)                        MP_TRACE_ERROR(err, LTM_ERR);
      }

      /* square */
      if (b > 1) {
         if ((err = mp_sqr(&g, &g)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR);
      }

      /* shift to next bit */
      b >>= 1;
   }

LTM_ERR:
   mp_clear(&g);
   return err;
}

#endif
