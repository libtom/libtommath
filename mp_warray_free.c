#include "tommath_private.h"
#ifdef MP_WARRAY_FREE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* static check that the multiplication won't overflow */
MP_STATIC_ASSERT(warray_free_sz_does_not_overflow, (sizeof(mp_word) * MP_WARRAY) >= MP_WARRAY)

static int s_warray_free(void)
{
   int ret = 0;
   size_t n;
   S_MP_WARRAY_LOCK();
   for (n = 0; n < s_mp_warray.allocated; ++n) {
      if (s_mp_warray.l_used[n].warray) {
         ret = -2;
         goto ERR_OUT;
      }
   }
   for (n = 0; n < s_mp_warray.allocated; ++n) {
      MP_FREE(s_mp_warray.l_free[n].warray, sizeof(mp_word) * MP_WARRAY);
      s_mp_warray.l_free[n].warray = NULL;
   }
   s_mp_warray_free(s_mp_warray.usable);
ERR_OUT:
   S_MP_WARRAY_UNLOCK();
   return ret;
}

int mp_warray_free(void)
{
   if (MP_HAS(MP_SMALL_STACK_SIZE)) return s_warray_free();
   return -1;
}

#endif
