#include "tommath_private.h"
#ifdef BN_MP_INIT_SIZE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * SPDX-License-Identifier: Unlicense
 */

/* init an mp_init for a given size */
int mp_init_size(mp_int *a, int size)
{
   int x;

   /* pad size so there are always extra digits */
   size += (MP_PREC * 2) - (size % MP_PREC);

   /* alloc mem */
   a->dp = (mp_digit *) XMALLOC((size_t)size * sizeof(mp_digit));
   if (a->dp == NULL) {
      return MP_MEM;
   }

   /* set the members */
   a->used  = 0;
   a->alloc = size;
   a->sign  = MP_ZPOS;

   /* zero the digits */
   for (x = 0; x < size; x++) {
      a->dp[x] = 0;
   }

   return MP_OKAY;
}
#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
