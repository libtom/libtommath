#include <tommath.h>
#ifdef BN_MP_ILOGB_D_C
int mp_ilogb_d(mp_int *a, int base, mp_int *c)
{
   int err, bits;
   mp_int b;

   if (a->sign == MP_NEG || base <= 0) {
      return MP_VAL;
   }

   err = MP_OKAY;

   if (base == 2) {
      bits = mp_count_bits(a) - 1;
      mp_set(c, bits);
      return err;
   }

   if ((err = mp_init(&b)) != MP_OKAY) {
      return err;
   }
   if ((err = mp_set_int(&b, base)) != MP_OKAY) {
      mp_clear(&b);
      return err;
   }
   if ((err = mp_ilogb(a, &b, c)) != MP_OKAY) {
      mp_clear(&b);
      return err;
   }

   mp_clear(&b);
   return MP_OKAY;
}
#endif

