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
 * The library is free for all purposes without any express
 * guarantee it works.
 */

#include <math.h>

int mp_set_double(mp_int *a, double d)
{
#if defined(__STDC_IEC_559__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
   uint64_t frac;
   int exp, res;
   union {
      double   dbl;
      uint64_t bits;
   } cast;
   cast.dbl = d;

   if (!isfinite(d)) {
      return MP_VAL;
   }
   exp = ((int)(cast.bits >> 52) & 0x7FF) - 1023 - 52;
   frac = (cast.bits & ((1ULL << 52) - 1)) | (1ULL << 52);

   res = mp_set_long_long(a, frac);
   if (res != MP_OKAY) {
      return res;
   }

   res = exp < 0 ? mp_div_2d(a, -exp, a, 0) : mp_mul_2d(a, exp, a);
   if ((cast.bits >> 63) && !mp_iszero(a)) {
      SIGN(a) = MP_NEG;
   }

   return MP_OKAY;
#else
#  error "No mp_set_double implementation is available"
#endif
}
#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
