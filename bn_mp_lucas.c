#include <tommath.h>
#ifdef BN_MP_LUCAS_C
static unsigned long _lucas(unsigned long n)
{
  unsigned long i = 2, j = 1, k, l;
  if(n==0)return i;
  for (k = 1; k < n; k++) {
    l = i + j;
    i = j;
    j = l;
  }
  return j;
}
/* An alternative could be sqrt(5*fib(n)^2+4*(-1)^n) */
int mp_lucas(unsigned long n, mp_int * c)
{
  mp_int fnm1, fnp1;
  int e;
  if(n < 41){
    mp_set_int(c,_lucas(n));
    return MP_OKAY;
  }
  if ((e = mp_init_multi(&fnm1, &fnp1, NULL)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_fibonacci(n - 1, &fnm1)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_fibonacci(n + 1, &fnp1)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_add(&fnm1, &fnp1, c)) != MP_OKAY) {
    return e;
  }
  return MP_OKAY;
}
#endif
