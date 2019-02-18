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
/* Minimum information needed */
#if ( (defined DBL_MAX) && (FLT_RADIX == 2) )

/* We can use a faster method if we have an IEEE compliant machine and a working stdint.h */
#if (((defined __STDC_IEC_559__) || (defined __GCC_IEC_559)) && (defined UINT64_MAX))

/* To avoid "magic numbers". Some people don't like them, even if their meaning is obvious */
/* Bits in mantissa without the implied bit  */
#define SIGNIFICANT (DBL_MANT_DIG - 1)
#define BIAS (DBL_MAX_EXP - 1)
/* Maximal unbiased exponent */
#define MAX_UNBIASED_EXPONENT ((2*DBL_MAX_EXP) - 1)
/* Length of a binary64 without sign bit */
#define PAYLOAD 63
int mp_set_double(mp_int *a, double b)
{
   uint64_t frac;
   int exp, res;
   union {
      double   dbl;
      uint64_t bits;
   } cast;
   cast.dbl = b;

   exp = (int)((unsigned)(cast.bits >> SIGNIFICANT) & (unsigned)MAX_UNBIASED_EXPONENT);
   frac = (cast.bits & ((1ULL << SIGNIFICANT) - 1ULL)) | (1ULL << SIGNIFICANT);

   if (exp == MAX_UNBIASED_EXPONENT) { /* +-inf, NaN */
      return MP_VAL;
   }
   exp -= BIAS + SIGNIFICANT;

   res = mp_set_long_long(a, frac);
   if (res != MP_OKAY) {
      return res;
   }

   res = (exp < 0) ? mp_div_2d(a, -exp, a, NULL) : mp_mul_2d(a, exp, a);
   if (res != MP_OKAY) {
      return res;
   }

   if (((cast.bits >> PAYLOAD) != 0ULL) && (mp_iszero(a) == MP_NO)) {
      SIGN(a) = MP_NEG;
   }

   return MP_OKAY;
}

#else
static double s_math_h_less_frexp(double x, int *exp)
{
   int exponent = 0, i;
   double high = 2.0;
   double low = 0.5;

   if (x >= 1.0) {
      for (i = 0; x >= high; i++) {
         exponent += (1 << i);
         x *= low;
         low *= low;
         high *= high;
      }
      if (x < 0.5) {
        while (x < 0.5) {
          x *= 2.0;
          exponent--;
        }
      }
      else {
         while (x >= 1.0) {
            x /= 2.0;
            exponent++;
         }
      }
   }
   *exp = exponent;
   return x;
}

int mp_set_double(mp_int *a, double b)
{
   int exp = 0, res = MP_OKAY, sign = MP_ZPOS;

   /* Check for NaN */
   /* TODO: there might be some ompilers who do not return true in case of NaN */
   if (b != b) {
      return MP_VAL;
   }

   if (b < 0.0) {
      b = b * (-1.0);
      sign = MP_NEG;
   }

   /* Check for infinity */
   if ( b > DBL_MAX) {
      return MP_VAL;
   }

   mp_zero(a);
   /* Numbers smaller than 1 truncate to zero */
   if (b < 1.0) {puts("b < 1.0");
      a->sign = sign;
      return MP_OKAY;
   }
   b = s_math_h_less_frexp(b, &exp);

   while (exp-- >= 0) {
      b *= 2.0;
      if (b >= 1.0) {
         if ((res = mp_add_d(a, 1, a)) != MP_OKAY) {
            return res;
         }
         b -= 1.0;
      }
      if (exp >= 0) {
         if ((res = mp_mul_2d(a, 1, a)) != MP_OKAY) {
            return res;
         }
      }
      if (b == 0.0) {
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
   return res;
}
#endif
#else
/* pragma message() not supported by several compilers (in mostly older but still used versions) */
#  ifdef _MSC_VER
#    pragma message("mp_set_double implementation is only available on platforms with IEEE754 floating point format.")
#    pragma message("At least DBL_MAX_EXP must be defined and set and, for now, FLT_RADIX must be 2.")
#  else
#    warning "mp_set_double implementation is only available on platforms with IEEE754 floating point format"
#    warning "At least DBL_MAX_EXP must be defined and set and, for now, FLT_RADIX must be 2."
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
