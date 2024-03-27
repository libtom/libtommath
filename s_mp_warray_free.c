#include "tommath_private.h"
#ifdef S_MP_WARRAY_FREE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

void s_mp_warray_free(size_t n)
{
   (void)n;
   MP_FREE(s_mp_warray.l_free, n * sizeof(*(s_mp_warray.l_free)));
   MP_FREE(s_mp_warray.l_used, n * sizeof(*(s_mp_warray.l_used)));
   s_mp_warray.l_free = NULL;
   s_mp_warray.l_used = NULL;
   s_mp_warray.allocated = 0;
   s_mp_warray.usable = 0;
}

#endif
