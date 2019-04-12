#include "tommath_private.h"
#ifdef BN_MP_INIT_SIZE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* init an mp_init for a given size */
int mp_init_size(mp_int *a, int size)
{
   /* ensure there are always at least MP_PREC digits extra on top */
   size += (MP_MIN_PREC + MP_PREC - 1) - ((size + (MP_PREC - 1)) % MP_PREC);

   /* alloc mem */
   a->dp = (mp_digit *) MP_CALLOC((size_t)size, sizeof(mp_digit));
   if (a->dp == NULL) {
      return MP_MEM;
   }

   /* set the members */
   a->used  = 0;
   a->alloc = size;
   a->sign  = MP_ZPOS;

   return MP_OKAY;
}
#endif
