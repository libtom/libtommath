#include "tommath_private.h"
#ifdef MP_NEG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* b = -a */
mp_err mp_neg(const mp_int *a, mp_int *b)
{
   if (a != b) {
      mp_err err;
      if ((err = mp_copy(a, b)) != MP_OKAY) {
         return err;
      }
   }

   b->sign = mp_iszero(b) || b->sign == MP_NEG ? MP_ZPOS : MP_NEG;

   return MP_OKAY;
}
#endif
