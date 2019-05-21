#include "tommath_private.h"
#ifdef BN_MP_TC_XOR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* two complement xor */
mp_err mp_tc_xor(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_err err = MP_OKAY;
   int bits, abits, bbits;
   mp_sign sa = a->sign, sb = b->sign;
   mp_int *mx = NULL, _mx, acpy, bcpy;

   if ((sa == MP_NEG) || (sb == MP_NEG)) {
      abits = mp_count_bits(a);
      bbits = mp_count_bits(b);
      bits = MP_MAX(abits, bbits);
      err = mp_init_set_int(&_mx, 1uL);
      if (err != MP_OKAY) {
         goto end;
      }

      mx = &_mx;
      err = mp_mul_2d(mx, bits + 1, mx);
      if (err != MP_OKAY) {
         goto end;
      }

      if (sa == MP_NEG) {
         err = mp_init(&acpy);
         if (err != MP_OKAY) {
            goto end;
         }

         err = mp_add(mx, a, &acpy);
         if (err != MP_OKAY) {
            mp_clear(&acpy);
            goto end;
         }
         a = &acpy;
      }
      if (sb == MP_NEG) {
         err = mp_init(&bcpy);
         if (err != MP_OKAY) {
            goto end;
         }

         err = mp_add(mx, b, &bcpy);
         if (err != MP_OKAY) {
            mp_clear(&bcpy);
            goto end;
         }
         b = &bcpy;
      }
   }

   err = mp_xor(a, b, c);

   if ((((sa == MP_NEG) && (sb != MP_NEG)) || ((sa != MP_NEG) && (sb == MP_NEG))) && (err == MP_OKAY)) {
      err = mp_sub(c, mx, c);
   }

end:
   if (a == &acpy) {
      mp_clear(&acpy);
   }

   if (b == &bcpy) {
      mp_clear(&bcpy);
   }

   if (mx == &_mx) {
      mp_clear(mx);
   }

   return err;
}
#endif
