#include "tommath_private.h"
#ifdef BN_MP_FACTORS_ZERO_C
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

/* Zero the elements of a factor-array (an array of mp_int's) and realloc to default
   size in that order */
#ifdef LTM_USE_EXTRA_FUNCTIONS
int mp_factors_zero(mp_factors *f)
{
   int i, e = MP_OKAY;
   mp_int *tmp;

   if (f->factors != NULL) {
      for (i = 0; i < f->length; i++) {
         mp_clear(&(f->factors[i]));
      }
   }

   tmp = OPT_CAST(mp_int) XREALLOC(f->factors, sizeof(*tmp) * LTM_TRIAL_GROWTH);
   if (tmp == NULL) {
      return MP_MEM;
   }

   f->factors = tmp;
   f->alloc = LTM_TRIAL_GROWTH;
   f->length = 0;

   return e;
}
#endif
#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
