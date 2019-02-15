#include "tommath_private.h"

#include <float.h>
#if ( (defined LDBL_MAX_EXP) && (FLT_RADIX == 2) )

static long double s_math_h_less_frexp(long double x, int *exp)
{
   int exponent = 0;

   while (x >= 1.0) {
      exponent++;
      if (exponent > LDBL_MAX_EXP) {
         break;
      }
      x /= 2.0;
   }
   *exp = exponent;
   return x;
}

int mp_set_long_double(mp_int *a, long double b)
{
   int exp = 0, res, sign = MP_ZPOS;
   /* Check for NaN */
   if (b != b) {
      return MP_VAL;
   }

   if (b < 0.0L) {
      b = b * (-1.0L);
      sign = MP_NEG;
   }
   mp_zero(a);
   /* Numbers smaller than 1 truncate to zero */
   if (b < 1.0L) {
      a->sign = sign;
      return MP_OKAY;
   }

   b = s_math_h_less_frexp(b, &exp);
   /* +/-inf if exp > LDBL_MAX_EXP */
   if (exp > LDBL_MAX_EXP) {
      return MP_VAL;
   }

   while (exp-- >= 0) {
      b *= 2.0L;
      if (b >= 1.0L) {
         if ((res = mp_add_d(a, 1, a)) != MP_OKAY) {
            return res;
         }
         b -= 1.0L;
      }

      if (exp >= 0) {
         if ((res = mp_mul_2d(a, 1, a)) != MP_OKAY) {
            return res;
         }
      }
      if (b == 0.0L) {
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
#    pragma message("The type 'long double' does not seem to be supported on your system.")
#    pragma message("If that is wrong please contact the team at https://github.com/libtommath/")
#  else
#    warning "The type 'long double' does not seem to be supported on your system."
#    warning "If that is wrong please contact the team at https://github.com/libtommath/"
#  endif
#endif
