#include "tommath_private.h"
#ifdef BN_MP_SIEVE_INIT_C
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

void mp_sieve_init(mp_sieve *sieve)
{
   sieve->base.content = NULL;
   sieve->segment.content = NULL;
   sieve->single_segment_a = 0;
}
#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
