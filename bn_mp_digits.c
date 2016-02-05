#include <tommath.h>
#ifdef BN_MP_DIGITS_C
#if  (defined MP_28BIT || defined MP_64BIT || defined MP_31BIT)
/*
 Returns the number of digits of base "base" of an mp_int.
 This is inexact (uses only 9 bits of precision) and should not
 be used outside of calculating memory needs!
 (e.g: both of the decimal numbers 9 and 10 fit in 4 bits but 
       10 needs two decimal digits)
*/
/*
  Table with precalculated logarithms of 1 - 64
     floor( (log([1 .. 64])/log(2)) * 10^9) )
*/
static mp_word logbases[65] = {
          0,          0, 1000000000, 1584962500, 2000000000,
 2321928094, 2584962500, 2807354922, 3000000000, 3169925001,

 3321928094, 3459431618, 3584962500, 3700439718, 3807354922,
 3906890595, 4000000000, 4087462841, 4169925001, 4247927513,

 4321928094, 4392317422, 4459431618, 4523561956, 4584962500,
 4643856189, 4700439718, 4754887502, 4807354922, 4857980995,

 4906890595, 4954196310, 5000000000, 5044394119, 5087462841,
 5129283016, 5169925001, 5209453365, 5247927513, 5285402218,

 5321928094, 5357552004, 5392317422, 5426264754, 5459431618,
 5491853096, 5523561956, 5554588851, 5584962500, 5614709844,

 5643856189, 5672425341, 5700439718, 5727920454, 5754887502,
 5781359713, 5807354922, 5832890014, 5857980995, 5882643049,

 5906890595, 5930737337, 5954196310, 5977279923, 6000000000,
};

long mp_digits(mp_int *a, int base)
{
   mp_word ten_pow_nine = 1e9;
   mp_word expanded_bits;
   long bits = mp_count_bits(a) + 1;
   if(bits < 0){
      return MP_VAL;
   }
   switch(base){
      case 0:  return MP_VAL;
      case 1:  return -1;
      case 2:  return bits + 1;
      case 4:  return bits/2 + 3;
      case 8:  return bits/3 + 3;
      case 16: return bits/4 + 3;
      case 32: return bits/5 + 3;
      case 64: return bits/6 + 3;
      default: break;
   }
   expanded_bits = (mp_word) bits;
   expanded_bits *= ten_pow_nine;
   return (expanded_bits / logbases[base]) + 5;
}
#else
#error "mp_digits needs 64 bit mp_word, use mp_radix_size instead"
#endif
#endif
