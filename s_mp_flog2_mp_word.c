#include "tommath_private.h"
#ifdef S_FLOG2_MP_WORD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

mp_word s_flog2_mp_word(mp_word value)
{
   mp_word r = 0u;
   while ((value >>= 1) != 0u) {
      r++;
   }
   return r;
}

#endif
