#include "tommath_private.h"
#ifdef BN_MP_FACTORS_INIT_C
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

/* Init factor-array (an array of mp_int's) */
#ifdef LTM_USE_EXTRA_FUNCTIONS
int mp_factors_init(mp_factors *f)
{
   f->factors = OPT_CAST(mp_int) XMALLOC(sizeof(mp_int) * (size_t)(LTM_TRIAL_GROWTH));
   if ((f->factors) == NULL) {
      return MP_MEM;
   }
   f->alloc = LTM_TRIAL_GROWTH;
   f->length = 0;
   return MP_OKAY;
}
#endif

#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
