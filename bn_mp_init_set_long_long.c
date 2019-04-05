#include "tommath_private.h"
#ifdef BN_MP_INIT_SET_LONG_LONG_C
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

/* initialize and set long long value */
int mp_init_set_long_long(mp_int *a, unsigned long long b)
{
   int err;
   if ((err = mp_init(a)) != MP_OKAY) {
      return err;
   }
   return mp_set_long_long(a, b);
}
#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
