#include "tommath_private.h"
#ifdef S_MP_IS_MUL_WRONG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



bool s_mp_is_mul_wrong(const mp_int *a, const mp_int *b, const mp_int *c)
{
   mp_word checksum_a, checksum_b, checksum_c, checksum_prod, carry;
   checksum_a = s_mp_compute_limb_checksum(a);
   checksum_b = s_mp_compute_limb_checksum(b);
   checksum_c = s_mp_compute_limb_checksum(c);

   checksum_prod = checksum_a * checksum_b;

   while (checksum_prod > MP_MASK) {
      carry = checksum_prod >> MP_DIGIT_BIT;
      checksum_prod = (checksum_prod & MP_MASK) + carry;
   }
   if (checksum_prod == MP_MASK) {
      checksum_prod = 0u;
   }
   if (checksum_prod != checksum_c) {
      return true;
   }
   return false;
}


#endif
