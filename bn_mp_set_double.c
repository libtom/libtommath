#include <tommath.h>
#ifdef BN_MP_SET_DOUBLE_C
// TODO: check if 'int' is large enough!
typedef union {
  struct {
    unsigned int a; // low in little endian
    unsigned int b; // high in little endian
  } w;
  double u;
} dblintern;

static int is_little_endian() {
    int i = 1;
    char *c = (char *)&i;

    if (c[0] == 1){
       return 1;
    }
    else {
       return 0;
    }
}
// reduced frexp (no checks nor balances!)
static double mp_frexp(double d, int *e){
  int little_endian;
  unsigned int high;
  
  dblintern v;

  little_endian = is_little_endian();

  if(little_endian){
    v.u = d;
    high = v.w.b;
  } else {
    v.u = d;
    high = v.w.a;
  }
  // get exponent
  // shift right 20 bits
  *e = high / 1048576;
  // extract exponent from sign+exponent
  *e %= 2048;
  // subtract bias
  *e -= 1022;
  // get fraction
  high = high % 1048576;
  // set exponent to bias
  high += 1071644672;
  // reassemble number
  if(little_endian){
    v.w.b = high;
  } else {
    v.w.a = high;
  }
  return v.u;
}

/* integer part of a double */
int mp_set_double(mp_int *c, double d, int rounding_mode)
{
   int expnt, res, sign;
   double frac;

   sign = (d < 0) ? MP_NEG : MP_ZPOS;
   mp_zero(c);
   d = (d < 0) ? d * -1.0 : d;
   if( d < 1 && d > 0 ){
     return MP_OKAY;
   }

   frac = mp_frexp(d, &expnt);
   if (frac == 0) {
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

#endif
