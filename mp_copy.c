#include "tommath_private.h"
#ifdef MP_COPY_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* copy, b = a */
mp_err mp_copy(const mp_int *a, mp_int *b)
{
   mp_err err = MP_OKAY;

   /* if dst == src do nothing */
   if (a == b) {
      return err;
   }

   /* grow dest */
   if ((err = mp_grow(b, a->used)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR);

   /* copy everything over and zero high digits */
   s_mp_copy_digs(b->dp, a->dp, a->used);
   s_mp_zero_digs(b->dp + a->used, b->used - a->used);
   b->used = a->used;
   b->sign = a->sign;

LTM_ERR:
   return err;
}
#endif
