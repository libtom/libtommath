#include "tommath_private.h"
#ifdef BN_MP_SET_FLOAT_C
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
#if ( (defined FLT_MAX_EXP) && (FLT_RADIX == 2) )

static float s_math_h_less_frexp(float x, int *exp)
{
   int exponent = 0;

   while (x >= 1.0) {
      exponent++;
      if (exponent > FLT_MAX_EXP) {
         break;
      }
      x /= 2.0;
   }
   *exp = exponent;
   return x;
}

int mp_set_float(mp_int *a, float b)
{
   int exp = 0, res, sign = MP_ZPOS;
   /* Check for NaN */
   if (b != b) {
      return MP_VAL;
   }

   if (b < 0.0f) {
      b = b * (-1.0f);
      sign = MP_NEG;
   }
   mp_zero(a);
   /* Numbers smaller than 1 truncate to zero */
   if (b < 1.0f) {
      a->sign = sign;
      return MP_OKAY;
   }

   b = s_math_h_less_frexp(b, &exp);
   /* +/-inf if exp > FLT_MAX_EXP */
   if (exp > FLT_MAX_EXP) {
      return MP_VAL;
   }

   while (exp-- >= 0) {
      b *= 2.0f;
      if (b >= 1.0f) {
         if ((res = mp_add_d(a, 1, a)) != MP_OKAY) {
            return res;
         }
         b -= 1.0f;
      }

      if (exp >= 0) {
         if ((res = mp_mul_2d(a, 1, a)) != MP_OKAY) {
            return res;
         }
      }
      if (b == 0.0f) {
         exp--;
         break;
      }
   }

   if (res != MP_OKAY) {
      return res;
   }

   res = (exp < 0) ? mp_div_2d(a, -exp, a, NULL) : mp_mul_2d(a, exp, a);
   if (res != MP_OKAY) {
      return res;
   }

   SIGN(a) = sign;

   return MP_OKAY;
}
#else
/* pragma message() not supported by several compilers (in mostly older but still used versions) */
#  ifdef _MSC_VER
#    pragma message("mp_set_float implementation is only available on platforms with IEEE754 floating point format.")
#    pragma message("At least FLT_MAX_EXP must be defined and set and, for now, FLT_RADIX must be 2.")
#  else
#    warning "mp_set_float implementation is only available on platforms with IEEE754 floating point format"
#    warning "At least FLT_MAX_EXP must be defined and set and, for now, FLT_RADIX must be 2."
#  endif
#endif
#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
