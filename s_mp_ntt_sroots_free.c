#include "tommath_private.h"
#ifdef S_MP_NTT_SROOTS_FREE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/* MP_16BIT is just too small, not worth it */
#if (MP_DIGIT_BIT > 16)
void s_mp_ntt_sroots_free(ntt_sroots *R)
{
   if (!R) {
      return;
   }
   MP_FREE(R->w, (size_t)R->m * sizeof(mp_digit));
   R->w = NULL;
   R->nstages = 0;
}
#endif


#endif
