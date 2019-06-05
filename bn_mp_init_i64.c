#include "tommath_private.h"
#ifdef BN_MP_INIT_I64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* initialize "a" with a signed 64-bit integer */
mp_err mp_init_i64(mp_int *a, int64_t b)
{
   mp_err err;
   if ((err = mp_init(a)) != 0) {
      return err;
   }
   mp_set_i64(a, b);
   return 0;
}


#endif
