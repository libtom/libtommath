#include "tommath_private.h"
#ifdef BN_MP_FACTORS_INIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Init factor-array (an array of mp_int's) */
int mp_factors_init(mp_factors *f)
{
   f->factors = (mp_int *) XCALLOC((size_t)(LTM_TRIAL_GROWTH), sizeof(mp_int));
   if ((f->factors) == NULL) {
      return MP_MEM;
   }
   f->alloc = LTM_TRIAL_GROWTH;
   f->length = 0;
   return MP_OKAY;
}

#endif

