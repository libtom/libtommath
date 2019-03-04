#include "tommath_private.h"
#ifdef BN_MP_RADIX_SIZE_BITS_C


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

/* returns size of ASCII representation of the number of size "bits" */
int mp_radix_size_bits(const int bits, const int radix, int *size)
{
   return mp_radix_size_ex(NULL, bits, radix, size);
}
#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */

