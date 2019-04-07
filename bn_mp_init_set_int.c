#include "tommath_private.h"
#ifdef BN_MP_INIT_SET_INT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* initialize and set a digit */
int mp_init_set_int(mp_int *a, unsigned long b)
{
   int err;
   if ((err = mp_init(a)) != MP_OKAY) {
      return err;
   }
   return mp_set_int(a, b);
}
#endif
