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

#if defined(__STDC_IEC_559__) || defined(__GCC_IEC_559)
/* defined by GNU C, SAS/C, and Stratus VOS C -- in that order */
#if ((defined __m68k__) || (defined __MC68K__) || (defined M68000))
static double s_math_h_less_frexp(double x, int *exp)
{
  int exponent = 0;
  while (x >= 1.0) {
    exponent++;
    /* exp > DBL_MAX_EXP, it is inf */
    if (exponent == 1025) {
      *exp = exponent;
      return x;
    }
    x /= 2.0;
  }

  *exp = exponent;
  return x;
}

static double s_pow(double d, int e){
   double t;

   if (e == 0) {
      return d;
   }
   t = 1.0;
   while (e > 1) {
      if (e % 2 == 0) {
         d *= d;
         e /= 2;
      } else {
         t *= d;
         d *= d;
         e = (e - 1)/2;
      }
   }
   return d * t;

}

int mp_set_double(mp_int *a, double b)
{
   uint64_t frac;
   int exp = 0, res, sign = 1;

   /* Check for NaN */
   if (b != b) {
      return MP_VAL;
   }

   if (b < 0) {
      b = b * (-1.0);
      sign = -1;
   }
   /* Numbers smaller than 1 truncate to zero */
   if (b < 1.0) {
      mp_zero(a);
      return MP_OKAY;
   }

   b = s_math_h_less_frexp(b, &exp);
   /* +/-inf if exp > DBL_MAX_EXP */
   if (exp == 1025) {
      return MP_VAL;
   }

   /* 52 bit mantissa plus the one implicit bit */
   b = b * s_pow(2.0,53);
   /* TODO: use proper rounding instead of truncating? */
   frac = (uint64_t) b;
   exp -= 53;

   res = mp_set_long_long(a, frac);
   if (res != MP_OKAY) {
      return res;
   }

   res = (exp < 0) ? mp_div_2d(a, -exp, a, NULL) : mp_mul_2d(a, exp, a);
   if (res != MP_OKAY) {
      return res;
   }

   if ( (sign < 0) && (mp_iszero(a) == MP_NO)) {
      SIGN(a) = MP_NEG;
   }

   return MP_OKAY;
}
#else

int mp_set_double(mp_int *a, double b)
{
   uint64_t frac;
   int exp, res;
   union {
      double   dbl;
      uint64_t bits;
   } cast;
   cast.dbl = b;

   exp = (int)((unsigned)(cast.bits >> 52) & 0x7FFU);
   frac = (cast.bits & ((1ULL << 52) - 1ULL)) | (1ULL << 52);

   if (exp == 0x7FF) { /* +-inf, NaN */
      return MP_VAL;
   }
   exp -= 1023 + 52;

   res = mp_set_long_long(a, frac);
   if (res != MP_OKAY) {
      return res;
   }

   res = (exp < 0) ? mp_div_2d(a, -exp, a, NULL) : mp_mul_2d(a, exp, a);
   if (res != MP_OKAY) {
      return res;
   }

   if (((cast.bits >> 63) != 0ULL) && (mp_iszero(a) == MP_NO)) {
      SIGN(a) = MP_NEG;
   }

   return MP_OKAY;
}
#endif
#else
/* pragma message() not supported by several compilers (in mostly older but still used versions) */
#  ifdef _MSC_VER
#    pragma message("mp_set_double implementation is only available on platforms with IEEE754 floating point format")
#  else
#    warning "mp_set_double implementation is only available on platforms with IEEE754 floating point format"
#  endif
#endif
#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
