#include <tommath.h>
#ifdef BN_MP_CATALAN_C
int mp_catalan(unsigned long n, mp_int *c){
  int e;
  if(n == 0 || n == 1){
    if( (e = mp_set_int(c,1LU) ) != MP_OKAY){
      return e;
    }
    return MP_OKAY;
  }
  /*  C(2^31) is already a very large number
      C(2^31) ~ 1.7593e1,292,913,972
      The author's box would need approximately 1 year and seven month
      computing time.
      The number of digits follow loosely the size of n here, e.g.
      C(2^32)  ~ 1.9303e2,585,827,958
      C(2^64)  ~ 2.5891e11,106,046,577,046,714,235
      C(2^128) ~ 1.2728e204,870,398,877,478,727,500,024,218,500,206,465,342
   */
  if(n >= ULONG_MAX/2){
    return MP_VAL;
  }
  if( (e = mp_binomial(2*n,n,c) ) != MP_OKAY){
    return e;
  }
  if((n+1) >= 1<<DIGIT_BIT){
    mp_int temp;
    if( (e = mp_init(&temp) ) != MP_OKAY){
      return e;
    }
    if( (e = mp_set_int(&temp,n+1) ) != MP_OKAY){
      return e;
    }
    if( (e = mp_div(c,&temp,c,NULL) ) != MP_OKAY){
      return e;
    }
    mp_clear(&temp);
  }
  else{
    if( (e = mp_div_d(c,n+1,c,NULL) ) != MP_OKAY){
      return e;
    }
  }
  return MP_OKAY;
}
#endif
