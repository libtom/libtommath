#include "tommath_private.h"
#ifdef MP_SHRINK_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* shrink a bignum */
mp_err mp_shrink(mp_int *a)
{
   size_t alloc = MP_MAX(MP_MIN_PREC, a->used);
   if (a->alloc != alloc) {
      mp_digit *dp = (mp_digit *) MP_REALLOC(a->dp,
                                             a->alloc * sizeof(mp_digit),
                                             alloc * sizeof(mp_digit));
      if (dp == NULL) {
         return MP_MEM;
      }
      a->dp    = dp;
      a->alloc = alloc;
   }
   return MP_OKAY;
}
#endif
