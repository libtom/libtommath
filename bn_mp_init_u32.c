#include "tommath_private.h"
#ifdef BN_MP_INIT_U32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* initialize "a" with an unsigned 32-bit integer */
mp_err mp_init_u32(mp_int *a, uint32_t b)
{
   mp_err err;
   if ((err = mp_init(a)) != 0) {
      return err;
   }
   mp_set_u32(a, b);
   return 0;
}



#endif
