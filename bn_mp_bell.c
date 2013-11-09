#include <tommath.h>
#ifdef BN_MP_BELL_C
int mp_bell(unsigned long n, mp_int * c)
{
  unsigned long i = 0;
  mp_int s, temp;
  int e;
  if ((e = mp_init_multi(&s, &temp, NULL)) != MP_OKAY) {
    return e;
  }
  while (i++ <= n) {
    if ((e = mp_stirling2(n, i, &temp)) != MP_OKAY) {
      return e;
    }
    if ((e = mp_add(&s, &temp, &s)) != MP_OKAY) {
      return e;
    }
  }
  mp_exch(&s, c);
  mp_clear_multi(&s, &temp, NULL);
  return MP_OKAY;
}
#endif
