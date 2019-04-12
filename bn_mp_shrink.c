#include "tommath_private.h"
#ifdef BN_MP_SHRINK_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* shrink a bignum */
int mp_shrink(mp_int *a)
{
   mp_digit *tmp;
   int alloc = MP_PREC;

   if (a->used > MP_PREC) {
      alloc = a->used;
      alloc += (MP_PREC - 1) - ((alloc + (MP_PREC - 1)) % MP_PREC);
   }

   if (a->alloc != alloc) {
      if ((tmp = (mp_digit *) MP_REALLOC(a->dp,
                                         (size_t)a->alloc * sizeof(mp_digit),
                                         (size_t)alloc * sizeof(mp_digit))) == NULL) {
         return MP_MEM;
      }
      a->dp    = tmp;
      a->alloc = alloc;
   }
   return MP_OKAY;
}
#endif
