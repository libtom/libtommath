#include "tommath_private.h"
#ifdef S_MP_WARRAY_GET_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

void *s_mp_warray_get(void)
{
   void *ret = NULL;
   size_t n;
   if (s_mp_warray.usable == 0) {
      if (mp_warray_init(1, false) != MP_OKAY)
         return NULL;
   }
   for (n = 0; n < s_mp_warray.allocated;) {
      if (s_mp_warray.l_free[n].warray == NULL) {
         n++;
         continue;
      }
      ret = s_mp_warray.l_free[n].warray;
      if (s_mp_cmpexch_n(&s_mp_warray.l_free[n].warray, &ret, NULL)) {
         s_mp_warray.l_used[n].warray = ret;
         goto LBL_OUT;
      }
      /* restart from the beginning if we missed a potential slot */
      n = 0;
   }
   ret = NULL;
   if (s_mp_warray.allocated + 1 > s_mp_warray.usable)
      goto LBL_OUT;
   ret = MP_CALLOC(MP_WARRAY, sizeof(mp_word));
   if (ret != NULL)
      s_mp_warray.l_used[s_mp_warray.allocated++].warray = ret;

LBL_OUT:
   return ret;
}

#endif
