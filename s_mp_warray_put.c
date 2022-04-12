#include "tommath_private.h"
#ifdef S_MP_WARRAY_PUT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

void s_mp_warray_put(void *w)
{
   size_t n;
   S_MP_WARRAY_LOCK();
   for (n = 0; n < s_mp_warray.allocated; ++n) {
      if (s_mp_warray.l_used[n].warray == w) {
         s_mp_warray.l_free[n] = s_mp_warray.l_used[n];
         s_mp_warray.l_used[n].warray = NULL;
         break;
      }
   }
   S_MP_WARRAY_UNLOCK();
}

#endif
