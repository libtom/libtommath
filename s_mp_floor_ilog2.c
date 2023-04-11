#include "tommath_private.h"
#ifdef S_MP_FLOOR_ILOG2_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

size_t s_mp_floor_ilog2(mp_word value)
{
   size_t r = 0u;
   while ((value >>= 1) != 0u) {
      r++;
   }
   return r;
}

#endif
