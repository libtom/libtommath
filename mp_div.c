#include "tommath_private.h"
#ifdef MP_DIV_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

mp_err mp_div(const mp_int *a, const mp_int *b, mp_int *c, mp_int *d)
{
   mp_err err;

   /* is divisor zero ? */
   if (MP_IS_ZERO(b)) {
      return MP_VAL;
   }

   /* if a < b then q = 0, r = a */
   if (mp_cmp_mag(a, b) == MP_LT) {
      if (d != NULL) {
         err = mp_copy(a, d);
      } else {
         err = MP_OKAY;
      }
      if (c != NULL) {
         mp_zero(c);
      }
      return err;
   }

   if (MP_HAS(S_MP_DIV_RECURSIVE)
       && (b->used > MP_KARATSUBA_MUL_CUTOFF)
       && (b->used <= ((a->used)/3*2))) {
      err = s_mp_div_recursive(a, b, c, d);
   } else if (MP_HAS(S_MP_DIV_SCHOOL)) {
      err = s_mp_div_school(a, b, c, d);
   } else {
      err = s_mp_div_small(a, b, c, d);
   }

   return err;
}
#endif

