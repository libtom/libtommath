#include <tommath.h>
#ifdef BN_MP_ISDIVISIBLE_C
int mp_isdivisible(mp_int *a, mp_int *d)
{
   int err;
   mp_int res;
   mp_init(&res);

   if ((err = mp_mod(a, d, &res)) != MP_OKAY) {
      return err;
   }
   if (mp_iszero(&res)) {
      return MP_YES;
   }
   return MP_NO;
}
#endif
