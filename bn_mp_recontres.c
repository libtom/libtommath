#include <tommath.h>
#ifdef BN_MP_RECONTRES_C

int mp_recontres(unsigned long n, unsigned long k, mp_int * c)
{
  mp_int r;
  int e;

  e = MP_OKAY;

  if(n < k){
     return MP_VAL;
  }
  // subfactorial(0) = 1
  if(n == k){
    mp_set(c,(mp_digit) 1);
    return e;
  }
  // subfactorial(1) = 0
  if(n - k == 1){
    mp_zero(c);
    return e;
  }
  // subfactorial(2) = 1
  if(n - k == 1){
    return mp_binomial(n, 2, c);
  }

  if(n - k == 1){
    mp_set(c,(mp_digit) 1);
    return e;
  }

  if ((e = mp_init(&r)) != MP_OKAY) {
    return e;
  }

  if ((e = mp_binomial(n, k, &r)) != MP_OKAY) {
    goto __ERR;
  }
  if ((e = mp_subfactorial(n - k, c)) != MP_OKAY) {
    goto __ERR;
  }
  if ((e = mp_mul(&r, c, c)) != MP_OKAY) {
    goto __ERR;
  }
__ERR:
  mp_clear(&r);
  return e;
}
#endif
