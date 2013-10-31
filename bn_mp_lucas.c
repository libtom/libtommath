#include <tommath.h>
#ifdef BN_MP_LUCAS_C
/* An alternative could be sqrt(5*fib(n)^2+4*(-1)^n) */
int mp_lucas(unsigned long n, mp_int * c)
{
  mp_int fnm1, fnp1;
  int e;
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
