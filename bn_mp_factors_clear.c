#include "tommath_private.h"
#ifdef BN_MP_FACTORS_CLEAR_C
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


/* Clear and free factor-array (an array of mp_int's) */
void mp_factors_clear(mp_factors *f)
{
   int i;
   /* free(NULL) should be possible. Says the standard. */
   if (f->factors != NULL) {
      for (i = 0; i < f->length; i++) { 
         mp_clear( &f->factors[i] ); 
      }
      free(f->factors);
      f->factors = NULL;
      f->alloc = 0;
      f->length = 0;
   }
}

#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
