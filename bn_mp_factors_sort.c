#include "tommath_private.h"
#ifdef BN_MP_FACTORS_SORT_C
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


/* Sort factor-array (an array of mp_int's) */
/* TODO: Sorts inline. Work on copy instead? */
void mp_factors_sort(mp_factors *f)
{
   int i, idx;
   mp_int tmp;

   /*
      It will be almost always be already sorted and even if it is unsorted
      it will just show a bushy tail. So insertion sort it is.
   */
   for (i = 1 ; i < f->length; i++) {
      idx = i;
      while ((idx > 0) && (mp_cmp(&(f->factors[idx-1]),&(f->factors[idx])) == MP_GT)) {
         mp_exch(&(f->factors[idx]), &tmp);
         mp_exch(&(f->factors[idx-1]), &(f->factors[idx]));
         mp_exch(&tmp, &(f->factors[idx-1]));
         idx--;
      }
   }
}

#endif
/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
