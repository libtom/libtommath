#include "tommath_private.h"
#ifdef BN_MP_SIEVE_CLEAR_C
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



/* Free memory of one sieve */
#ifdef LTM_USE_EXTRA_FUNCTIONS
static void clear_one(mp_single_sieve *sieve){
   if (sieve->content != NULL) {
      free(sieve->content);
   }
}

void mp_sieve_clear(mp_sieve *sieve)
{
   clear_one(&(sieve->base));
   clear_one(&(sieve->segment));
}
#endif
#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
