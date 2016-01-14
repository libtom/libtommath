#include <tommath.h>
#ifdef BN_MP_RISING_FACTORIAL_C
int mp_rising_factorial(unsigned long n, unsigned long k, mp_int *c)
{
   int e;
   if ((e = mp_falling_factorial(n - k + 1, k, c)) != MP_OKAY) {
      return e;
   }
   return MP_OKAY;
}
#endif
