#include "tommath_private.h"
#ifdef MP_ADD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* high level addition (handles signs) */
mp_err mp_add(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_err err = MP_OKAY;
   /* handle two cases, not four */
   if (a->sign == b->sign) {
      /* both positive or both negative */
      /* add their magnitudes, copy the sign */
      c->sign = a->sign;
      if ((err = s_mp_add(a, b, c)) != MP_OKAY)                         MP_TRACE_ERROR(err, LTM_ERR);
      return err;
   }

   /* one positive, the other negative */
   /* subtract the one with the greater magnitude from */
   /* the one of the lesser magnitude. The result gets */
   /* the sign of the one with the greater magnitude. */
   if (mp_cmp_mag(a, b) == MP_LT) {
      MP_EXCH(const mp_int *, a, b);
   }

   c->sign = a->sign;
   if ((err = s_mp_sub(a, b, c)) != MP_OKAY)                            MP_TRACE_ERROR(err, LTM_ERR);

LTM_ERR:
   return err;
}

#endif
