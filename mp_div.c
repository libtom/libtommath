#include "tommath_private.h"
#ifdef MP_DIV_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

mp_err mp_div(const mp_int *a, const mp_int *b, mp_int *c, mp_int *d)
{
   mp_err err = MP_OKAY;

   /* is divisor zero ? */
   if (mp_iszero(b)) {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   /* if a < b then q = 0, r = a */
   if (mp_cmp_mag(a, b) == MP_LT) {
      if (d != NULL) {
         if ((err = mp_copy(a, d)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR);
      }
      if (c != NULL) {
         mp_zero(c);
      }
      return MP_OKAY;
   }

   if (MP_HAS(S_MP_DIV_RECURSIVE)
       && (b->used > (2 * MP_MUL_KARATSUBA_CUTOFF))
       && (b->used <= ((a->used/3)*2))) {
      if ((err = s_mp_div_recursive(a, b, c, d)) != MP_OKAY)            MP_TRACE_ERROR(err, LTM_ERR);
   } else if (MP_HAS(S_MP_DIV_SCHOOL)) {
      if ((err = s_mp_div_school(a, b, c, d)) != MP_OKAY)               MP_TRACE_ERROR(err, LTM_ERR);
   } else if (MP_HAS(S_MP_DIV_SMALL)) {
      if ((err = s_mp_div_small(a, b, c, d)) != MP_OKAY)                MP_TRACE_ERROR(err, LTM_ERR);
   } else {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

LTM_ERR:
   return err;
}
#endif
