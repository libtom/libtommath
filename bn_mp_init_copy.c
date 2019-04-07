#include "tommath_private.h"
#ifdef BN_MP_INIT_COPY_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* creates "a" then copies b into it */
int mp_init_copy(mp_int *a, const mp_int *b)
{
   int     res;

   if ((res = mp_init_size(a, b->used)) != MP_OKAY) {
      return res;
   }

   if ((res = mp_copy(b, a)) != MP_OKAY) {
      mp_clear(a);
   }

   return res;
}
#endif
