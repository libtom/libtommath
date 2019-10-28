#include "tommath_private.h"
#ifdef MP_GROW_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* grow as required */
mp_err mp_grow(mp_int *a, size_t size)
{
   /* if the alloc size is smaller alloc more ram */
   if (a->alloc < size) {
      /* reallocate the array a->dp
       *
       * We store the return in a temporary variable
       * in case the operation failed we don't want
       * to overwrite the dp member of a.
       */
      mp_digit *dp = (mp_digit *) MP_REALLOC(a->dp,
                                             a->alloc * sizeof(mp_digit),
                                             size * sizeof(mp_digit));
      if (dp == NULL) {
         /* reallocation failed but "a" is still valid [can be freed] */
         return MP_MEM;
      }

      /* reallocation succeeded so set a->dp */
      a->dp = dp;

      /* zero excess digits */
      MP_ZERO_DIGITS_NEW(a->dp + a->alloc, a->dp + size);
      a->alloc = size;
   }
   return MP_OKAY;
}
#endif
