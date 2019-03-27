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
#ifdef LTM_USE_EXTRA_FUNCTIONS
/* TODO: Sorts inline. Work on copy instead? */
int mp_factors_sort(mp_factors *f)
{
   int i, idx, e = MP_OKAY;
   mp_int tmp;

   if ((e = mp_init(&tmp)) != MP_OKAY) {
      return e;
   }

   /*
      It will be almost always be already sorted and even if it is unsorted
      it will just show a bushy tail. So insertion sort it is.
   */
   for (i = 1 ; i < f->length; i++) {
      idx = i;
      while ((idx > 0) && (mp_cmp(&(f->factors[idx-1]),&(f->factors[idx])) == MP_GT)) {
         if ((e = mp_copy(&(f->factors[idx]), &tmp)) != MP_OKAY) {
            goto LTM_ERR;
         }
         if ((e = mp_copy(&(f->factors[idx-1]), &(f->factors[idx]))) != MP_OKAY) {
            goto LTM_ERR;
         }
         if ((e = mp_copy(&tmp, &(f->factors[idx-1]))) != MP_OKAY) {
            goto LTM_ERR;
         }
         idx--;
      }
   }
LTM_ERR:
   mp_clear(&tmp);
   return e;
}

#endif
#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
