#include "tommath_private.h"
#ifdef BN_MP_GET_LONG_DOUBLE_C
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
#include <float.h>
#ifdef LDBL_MAX
/* This function is independent of the implementation of the floating point type */
long double mp_get_long_double(const mp_int *a)
{
   int i;
   long double d = 0.0, fac = 1.0;
   for (i = 0; i < DIGIT_BIT; ++i) {
      fac *= 2.0;
   }
   for (i = a->used; i --> 0;) {
      d = (d * fac) + (long double)DIGIT(a, i);
   }
   return (a->sign == MP_NEG) ? -d : d;
}
#else
/* pragma message() not supported by several compilers (in mostly older but still used versions) */
#  ifdef _MSC_VER
#    pragma message("The type 'long double' does not seem to be supported on your system.")
#    pragma message("If that is wrong please contact the team at https://github.com/libtommath/")
#  else
#    warning "The type 'long double' does not seem to be supported on your system."
#    warning "If that is wrong please contact the team at https://github.com/libtommath/"
#  endif
#endif

#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
