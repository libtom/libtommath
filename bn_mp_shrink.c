#include "tommath_private.h"
#ifdef BN_MP_SHRINK_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* shrink a bignum */
int mp_shrink(mp_int *a)
{
   mp_digit *tmp;
   int used = 1;

   if (a->used > 0) {
      used = a->used;
   }

   if (a->alloc != used) {
      if ((tmp = (mp_digit *) MP_REALLOC(a->dp,
                                         (size_t)a->alloc * sizeof(mp_digit),
                                         (size_t)used * sizeof(mp_digit))) == NULL) {
         return MP_MEM;
      }
      a->dp    = tmp;
      a->alloc = used;
   }
   return MP_OKAY;
}
#endif
