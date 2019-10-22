#include "tommath_private.h"
#ifdef S_MP_LOG10_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifdef MP_16BIT
static mp_err s_mp_add_u32(mp_int *a, uint32_t x)
{
   mp_int t;
   mp_err err;
   if ((err = mp_init_u32(&t, x)) != MP_OKAY) {
      return err;
   }
   err = mp_add(a, &t, a);
   mp_clear(&t);
   return err;
}
#endif

mp_err s_mp_log10(const mp_int *a, uint32_t *c)
{
   /*
       floor(2^64/log_2(10)) = 5553023288523357132

       We need that much precision. Example:

         log_2(x) = log(x)/log(2)
         198096465/log_2(10) = 59632978.000000002598316594477929217520

        Which are more than 16 decimal digits, so with BINARY_64 (C's "double")
        a 'ceil(198096465/log_2(10))' would result wrongly in 59632978.0.
    */
   mp_err err;
   mp_int bit_count, k;

#ifdef MP_16BIT
   if ((err = mp_init_u32(&k, 0x4d104d42UL)) != MP_OKAY) {
      return err;
   }
   if ((err = mp_mul_2d(&k, 32, &k)) != MP_OKAY) {
      goto LBL_ERR1;
   }
   if ((err = s_mp_add32(&k, 0x7de7fbccUL)) != MP_OKAY) {
      goto LBL_ERR1;
   }
#else
   if ((err = mp_init_u64(&k, 0x4d104d427de7fbccULL)) != MP_OKAY) {
      return err;
   }
#endif

   if ((err = mp_init_l(&bit_count, mp_count_bits(a) - 1))  != MP_OKAY) {
      goto LBL_ERR1;
   }
   if ((err = mp_mul(&bit_count, &k, &k)) != MP_OKAY) {
      goto LBL_ERR2;
   }
   if ((err = mp_div_2d(&k, 64, &k, NULL)) != MP_OKAY) {
      goto LBL_ERR2;
   }
   *c = mp_get_u32(&k);

LBL_ERR2:
   mp_clear(&bit_count);
LBL_ERR1:
   mp_clear(&k);
   return err;
}

#endif
