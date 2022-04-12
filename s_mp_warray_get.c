#include "tommath_private.h"
#ifdef S_MP_WARRAY_GET_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

void *s_mp_warray_get(void)
{
   void *ret = NULL;
   size_t n;
   S_MP_WARRAY_LOCK();
   if (s_mp_warray.usable == 0) {
      if (mp_warray_init(MP_WARRAY_NUM, false, NULL) != MP_OKAY)
         return NULL;
   }
   for (n = 0; n < s_mp_warray.allocated; ++n) {
      if (s_mp_warray.l_free[n].warray) {
         s_mp_warray.l_used[n] = s_mp_warray.l_free[n];
         s_mp_warray.l_free[n].warray = NULL;
         ret = s_mp_warray.l_used[n].warray;
         goto LBL_OUT;
      }
   }
   if (s_mp_warray.allocated + 1 > s_mp_warray.usable)
      goto LBL_OUT;
   ret = MP_CALLOC(MP_WARRAY, sizeof(mp_word));
   s_mp_warray.l_used[s_mp_warray.allocated++].warray = ret;

LBL_OUT:
   S_MP_WARRAY_UNLOCK();
   return ret;
}

#endif
