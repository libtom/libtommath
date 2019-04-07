#include "tommath_private.h"
#ifdef BN_MP_FACTORS_ADD_C
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


/* Add one element (mp_int) to a factor-array (an array of mp_int's) */
int mp_factors_add(const mp_int *a, mp_factors *f)
{
   int new_size = f->alloc;
   mp_int *tmp;
   mp_int t;
   if (f->alloc <= f->length) {
      new_size += LTM_TRIAL_GROWTH;
      tmp = (mp_int *) XREALLOC(f->factors, sizeof(*tmp) * f->alloc,
                                sizeof(*tmp) * (size_t)(new_size));
      if (tmp == NULL) {
         return MP_MEM;
      }
      f->factors = tmp;
      f->alloc = new_size;
   }
   mp_init(&t);
   mp_copy(a,&t);
   f->factors[f->length] = t;
   f->length++;
   return MP_OKAY;
}

#endif
/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
