#include "tommath_private.h"
#ifdef BN_MP_GET_DOUBLE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 */

double mp_get_double(const mp_int *a)
{
   int i;
   double d = 0, fac = 1;
   for (i = 0; i < DIGIT_BIT; ++i) {
      fac *= 2;
   }
   for (i = USED(a); i --> 0;) {
      d = d * fac + (double)DIGIT(a, i);
   }
   return mp_isneg(a) ? -d : d;
}
#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
