#include "tommath_private.h"
#ifdef S_MP_WARRAY_PUT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

void s_mp_warray_put(void *w)
{
   size_t n, allocated = s_mp_warray.allocated;
   for (n = 0; n < allocated; ++n) {
      if (s_mp_warray.l_used[n].warray == w) {
         s_mp_cmpexch_n(&s_mp_warray.l_used[n].warray, &w, NULL);
         s_mp_warray.l_free[n].warray = w;
         break;
      }
   }
}

#endif
