#include "tommath_private.h"
#ifdef S_MP_RADIX_SIZE_RADIX_10_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#define LTM_RADIX_SIZE_SCALE 64
int s_mp_radix_size_radix_10(const mp_int *a, size_t *size)
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
   mp_int bi_bit_count, bi_k, t;
   int bit_count;

#ifdef MP_16BIT
#define LTM_RADIX_SIZE_CONST_SHIFT 32
   const uint32_t inv_log_2_10[2] = {0x4d104d42UL, 0x7de7fbccUL};
#endif
   if ((err = mp_init_multi(&bi_bit_count, &bi_k, &t, NULL)) != MP_OKAY) {
      return err;
   }
#ifdef MP_16BIT
   mp_set_u32(&t, inv_log_2_10[0]);
   if ((err = mp_mul_2d(&bi_k, LTM_RADIX_SIZE_CONST_SHIFT, &bi_k)) != MP_OKAY) {
      goto LTM_E1;
   }
   if ((err = mp_add(&bi_k, &t, &bi_k)) != MP_OKAY) {
      goto LTM_E1;
   }
   mp_set_u32(&t, inv_log_2_10[1]);
   if ((err = mp_mul_2d(&bi_k, LTM_RADIX_SIZE_CONST_SHIFT, &bi_k)) != MP_OKAY) {
      goto LTM_E1;
   }
   if ((err = mp_add(&bi_k, &t, &bi_k)) != MP_OKAY) {
      goto LTM_E1;
   }
#else
   mp_set_u64(&bi_k, 0x4d104d427de7fbccULL);
#endif
   bit_count = mp_count_bits(a) + 1;
   mp_set_l(&bi_bit_count, bit_count);
   if ((err = mp_mul(&bi_bit_count, &bi_k, &bi_k)) != MP_OKAY) {
      goto LTM_E1;
   }
   if ((err = mp_div_2d(&bi_k, LTM_RADIX_SIZE_SCALE, &bi_k, NULL)) != MP_OKAY) {
      goto LTM_E1;
   }
   *size = (size_t)mp_get_ul(&bi_k);
   *size += 2u + (size_t)(a->sign == MP_NEG);
LTM_E1:
   mp_clear_multi(&bi_bit_count, &bi_k, &t, NULL);
   return err;
}


#endif
