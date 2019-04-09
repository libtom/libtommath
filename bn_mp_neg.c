#include "tommath_private.h"
#ifdef BN_MP_NEG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* b = -a */
int mp_neg(const mp_int *a, mp_int *b)
{
   int     res;
   if (a != b) {
      if ((res = mp_copy(a, b)) != MP_OKAY) {
         return res;
      }
   }

   if (!MP_IS_ZERO(b)) {
      b->sign = (a->sign == MP_ZPOS) ? MP_NEG : MP_ZPOS;
   } else {
      b->sign = MP_ZPOS;
   }

   return MP_OKAY;
}
#endif
