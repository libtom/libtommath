#include "tommath_private.h"
#ifdef BN_S_MP_RADIX_SIZE_RADIX_10_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


#define LTM_RADIX_SIZE_SCALE 64
#define LTM_RADIX_SIZE_CONST_SHIFT 16
int s_mp_radix_size_radix_10(const mp_int *a, int *size)
{
   mp_err err;
   /*
       floor(2^64/log_2(10)) = 5553023288523357132

       We need that much precision. Example:

         log_2(x) = log(x)/log(2)
         198096465/log_2(10) = 59632978.000000002598316594477929217520

        Which are more than 16 decimal digits, so with BINARY_64 (C's "double")
        a 'ceil(198096465/log_2(10))' would result wrongly in 59632978.0.
    */
   /*
       TODO: This is safe for MP_8BIT, change to
                const uint32_t inv_log_2_10[2] = {0x4d104d42U, 0x7de7fbccU}
             or branch even further with
                const uint64_t inv_log_2_10 = {0x4d104d427de7fbccULL};
             when MP_8BIT got the boot.
    */
   const uint16_t inv_log_2_10[4] = {0x4d10u, 0x4d42u, 0x7de7u, 0xfbccu};
   mp_int bi_bit_count, bi_k, t;
   int i, bit_count;
   if ((err = mp_init_multi(&bi_bit_count, &bi_k, &t, NULL)) != MP_OKAY) {
      return err;
   }
   for (i = 0; i < (LTM_RADIX_SIZE_SCALE/LTM_RADIX_SIZE_CONST_SHIFT); i++) {
      /* Safe for MP_8BIT. Change to mp_set_u32 and/or mp_set_u64 if MP_8BIT gets the boot. */
      mp_set_l(&t, (long)inv_log_2_10[i]);
      if ((err = mp_mul_2d(&bi_k, LTM_RADIX_SIZE_CONST_SHIFT, &bi_k)) != MP_OKAY) {
         goto LTM_E1;
      }
      if ((err = mp_add(&bi_k, &t, &bi_k)) != MP_OKAY) {
         goto LTM_E1;
      }
   }
   bit_count = mp_count_bits(a) + 1;
   mp_set_l(&bi_bit_count, bit_count);
   if ((err = mp_mul(&bi_bit_count, &bi_k, &bi_k)) != MP_OKAY) {
      goto LTM_E1;
   }
   if ((err = mp_div_2d(&bi_k, LTM_RADIX_SIZE_SCALE, &bi_k, NULL)) != MP_OKAY) {
      goto LTM_E1;
   }
   *size = (int)mp_get_l(&bi_k);
   *size += 2 + (a->sign == MP_NEG);
LTM_E1:
   mp_clear_multi(&bi_bit_count, &bi_k, &t, NULL);
   return err;
}
#endif
