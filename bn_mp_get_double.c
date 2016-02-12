#include <tommath.h>
#ifdef BN_MP_GET_DOUBLE_C

#ifndef DBL_MANT_DIG
#define DBL_MANT_DIG 53
#endif

static double st_pow(double d, int e){
  double t;

  if (e == 0) {
     return d;
  }
  t = 1.0;
  while(e > 1){
     if(e % 2 == 0){
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

/* Convert to double, assumes IEEE-754 conforming double. From code by
   gerdr (Gerhard R.) https://github.com/gerdr */
int mp_get_double(mp_int *a, double *d)
{
   int digits_needed, i, limit;
   double multiplier;

   /* digits_needed * DIGIT_BIT >= 64 bit */
   digits_needed = ((DBL_MANT_DIG + DIGIT_BIT) / DIGIT_BIT) + 1;

   multiplier = (double)(MP_MASK + 1);

   *d = 0.0;

   /* Could be assumed, couldn't it? */
   mp_clamp(a);

   i = a->used;

   limit = (i <= digits_needed) ? 0 : i - digits_needed;

   while (i-- > limit) {
      *d += a->dp[i];
      *d *= multiplier;
   }

   if (a->sign == MP_NEG) {
      *d *= -1.0;
   }

   *d *= st_pow(2.0, i * DIGIT_BIT);

   return MP_OKAY;
}

#endif
