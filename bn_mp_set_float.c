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

/* We can use a faster method if we have an IEEE compliant machine and a working stdint.h */
#if ( (defined LTM_NEARLY_IEC_559 ) && (defined UINT32_MAX) )
int mp_set_float(mp_int *a, float b)
{
   uint64_t frac;
   int exp, res = MP_OKAY;
   union {
      float   dbl;
      uint32_t bits;
   } cast;
   cast.dbl = b;

   exp = (int)((unsigned)(cast.bits >> (FLT_MANT_DIG - 1)) & (unsigned)((2*FLT_MAX_EXP) - 1));
   frac = (cast.bits & ((1UL << (FLT_MANT_DIG - 1)) - 1UL)) | (1UL << (FLT_MANT_DIG - 1));

   if (exp == ((2*FLT_MAX_EXP) - 1)) { /* +-inf, NaN */
      return MP_VAL;
   }
   exp -= (FLT_MAX_EXP - 1) + (FLT_MANT_DIG - 1);

   res = mp_set_long(a, frac);
   if (res != MP_OKAY) {
      return res;
   }

   res = (exp < 0) ? mp_div_2d(a, -exp, a, NULL) : mp_mul_2d(a, exp, a);
   if (res != MP_OKAY) {
      return res;
   }
   /* 31 = number of bits without sign-bit in binary32 */
   if (((cast.bits >> 31) != 0ULL) && (!IS_ZERO(a))) {
      a->sign = MP_NEG;
   }

   return MP_OKAY;
}
#else
static float s_math_h_less_frexp(float x, int *exp)
{
   int exponent = 0, i;
   float high = 2.0f;
   float low = 0.5f;

   if (x >= 1.0f) {
      for (i = 0; x >= high; i++) {
         exponent += (1 << i);
         x *= low;
         low *= low;
         high *= high;
      }
      if (x < 0.5f) {
         while (x < 0.5f) {
            x *= 2.0f;
            exponent--;
         }
      } else {
         while (x >= 1.0f) {
            x /= 2.0f;
            exponent++;
         }
      }
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

   /* Check for infinity */
   if (b > FLT_MAX) {
      return MP_VAL;
   }

   mp_zero(a);
   /* Numbers smaller than 1 truncate to zero */
   if (b < 1.0f) {
      a->sign = sign;
      return MP_OKAY;
   }

   b = s_math_h_less_frexp(b, &exp);

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

   a->sign = sign;

   return MP_OKAY;
}
#endif

#else
/* pragma message() not supported by several compilers (in mostly older but still used versions) */
#  ifdef _MSC_VER
#    pragma message("mp_set_float implementation is only available on platforms with IEEE754 floating point format.")
#    pragma message("At least FLT_MAX_EXP must be defined and set and, for now, FLT_RADIX must be 2.")
#  else
#    warning "mp_set_float implementation is only available on platforms with IEEE754 floating point format"
#    warning "At least FLT_MAX_EXP must be defined and set and, for now, FLT_RADIX must be 2."
#  endif

#if (FLT_RADIX == 16)
#  ifdef _MSC_VER
#    pragma message("No radices other than two are supported. IBM's z/OS uses IEEE-754 compliant floats")
#    pragma message("with the compiler option FLOAT(IEEE)")
#  else
#    warning "No radices other than two are supported. IBM's z/OS uses IEEE-754 compliant floats"
#    warning "with the compiler option FLOAT(IEEE)"
#  endif
#endif
#endif
#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
