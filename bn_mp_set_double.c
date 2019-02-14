#include "tommath_private.h"
#ifdef BN_MP_SET_DOUBLE_C
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
#if ((defined DBL_MAX_EXP) && (FLT_RADIX == 2))
static double s_math_h_less_frexp(double x, int *exp)
{
  int exponent = 0;
  while (x >= 1.0) {
    exponent++;
    /* exp > DBL_MAX_EXP + 1, it is inf */
    if (exponent == (DBL_MAX_EXP + 1)) {
      *exp = exponent;
      return x;
    }
    x /= 2.0;
  }

  *exp = exponent;
  return x;
}


int mp_set_double(mp_int *c, double d)
{
   int expnt, res, sign = MP_ZPOS;
   double frac;


   if (d != d) {
      return MP_VAL;
   }
   if (d < 0) {
      d = d * (-1.0);
      sign = MP_NEG;
   }
   /* We do not work on copy, hence clear output memory */
   mp_zero(c);
   /* Integers only */
   if (d < 1.0) {
      c->sign = sign;
      return MP_OKAY;
   }

   frac = s_math_h_less_frexp(d, &expnt);
   /* +/-inf if exp > DBL_MAX_EXP */
   if (expnt == (DBL_MAX_EXP + 1)) {
      return MP_VAL;
   }

   if (frac == 0.0) {
      c->sign = sign;
      return MP_OKAY;
   }

   while (expnt-- >= 0) {
      frac *= 2.0;
      if (frac >= 1.0) {
         if ((res = mp_add_d(c, 1, c)) != MP_OKAY) {
            return res;
         }
         frac -= 1.0;
      }
      if (expnt > 0) {
         if ((res = mp_mul_2d(c, 1, c)) != MP_OKAY) {
            return res;
         }
      }
   }
   c->sign = sign;
   return MP_OKAY;
}
#else
/* pragma message() not supported by several compilers (in mostly older but still used versions) */
#  ifdef _MSC_VER
#    pragma message("mp_set_double implementation is only available on platforms with IEEE754 floating point format.")
#    pragma message("At least DBL_MAX_EXP must be defined and set and, for now, FLT_RADIX must be 2.")
#  else
#    warning "mp_set_double implementation is only available on platforms with IEEE754 floating point format"
#    warning "At least DBL_MAX_EXP must be defined and set and, for now, FLT_RADIX must be 2."
#  endif
#endif
#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
