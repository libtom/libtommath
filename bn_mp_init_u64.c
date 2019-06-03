#include "tommath_private.h"
#ifdef BN_MP_INIT_U64_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* initialize "a" with an unsigned 64-bit integer */
mp_err mp_init_u64(mp_int *a, uint64_t b)
{
   mp_err err;
   if ((err = mp_init(a)) != 0) {
      return err;
   }
   mp_set_u64(a, b);
   return 0;
}



#endif
