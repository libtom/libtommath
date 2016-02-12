#include <tommath.h>
#ifdef BN_MP_JACOBSTHAL_LUCAS_C
int mp_jacobsthal_lucas(unsigned long n, mp_int *c)
{
   mp_int r;
   int e;
   if ((e = mp_init(&r)) != MP_OKAY) {
      return e;
   }
   mp_set(&r, 1);
   if (n & 0x1) {
      if ((e = mp_mul_2d(&r, n, &r)) != MP_OKAY) {
         return e;
      }
      if ((e = mp_sub_d(&r, 1, c)) != MP_OKAY) {
         return e;
      }
   } else {
      if ((e = mp_mul_2d(&r, n, &r)) != MP_OKAY) {
         return e;
      }
      if ((e = mp_add_d(&r, 1, c)) != MP_OKAY) {
         return e;
      }
   }
   mp_clear(&r);
   return MP_OKAY;
}
#endif
