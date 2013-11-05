#include <tommath.h>
#ifdef BN_MP_LEONARDO_C
int mp_leonardo(unsigned long n, mp_int * c)
{
  mp_int r;
  if ((e = mp_init(&r)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_fibonacci(n + 1, &r)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_mul_2d(&r, 1, &r)) != MP_OKAY) {
    return e;
  }
  if ((e = mp_sub_d(&r, 1, c)) != MP_OKAY) {
    return e;
  }
  mp_clear(&r);
  return MP_OKAY;
}
#endif
