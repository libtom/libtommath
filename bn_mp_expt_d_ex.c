#include "tommath_private.h"
#ifdef BN_MP_EXPT_D_EX_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* calculate c = a**b  using a square-multiply algorithm */
mp_err mp_expt_d_ex(const mp_int *a, mp_digit b, mp_int *c, int fast)
{
   mp_err err;
   unsigned int x;

   mp_int  g;

   if ((err = mp_init_copy(&g, a)) != MP_OKAY) {
      return err;
   }

   /* set initial result */
   mp_set_u(c, 1u);

   if (fast != 0) {
      while (b > 0u) {
         /* if the bit is set multiply */
         if ((b & 1u) != 0u) {
            if ((err = mp_mul(c, &g, c)) != MP_OKAY) {
               mp_clear(&g);
               return err;
            }
         }

         /* square */
         if (b > 1u) {
            if ((err = mp_sqr(&g, &g)) != MP_OKAY) {
               mp_clear(&g);
               return err;
            }
         }

         /* shift to next bit */
         b >>= 1;
      }
   } else {
      for (x = 0; x < (unsigned)MP_DIGIT_BIT; x++) {
         /* square */
         if ((err = mp_sqr(c, c)) != MP_OKAY) {
            mp_clear(&g);
            return err;
         }

         /* if the bit is set multiply */
         if ((b & ((mp_digit)1 << (MP_DIGIT_BIT - 1))) != 0u) {
            if ((err = mp_mul(c, &g, c)) != MP_OKAY) {
               mp_clear(&g);
               return err;
            }
         }

         /* shift to next bit */
         b <<= 1;
      }
   } /* if ... else */

   mp_clear(&g);
   return MP_OKAY;
}
#endif
