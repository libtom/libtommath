#include "tommath_private.h"
#ifdef S_MP_IS_DIV_WRONG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



bool s_mp_is_div_wrong(const mp_int *a, const mp_int *b, const mp_int *q, const mp_int *r)
{
   mp_word checksum_a, checksum_b, checksum_q, checksum_r, checksum_should_be_a, carry;

   checksum_a = s_mp_compute_limb_checksum(a);
   checksum_b = s_mp_compute_limb_checksum(b);
   checksum_q = s_mp_compute_limb_checksum(q);
   checksum_r = s_mp_compute_limb_checksum(r);

   checksum_should_be_a = (checksum_q * checksum_b) + checksum_r;

   while (checksum_should_be_a > MP_MASK) {
      carry = checksum_should_be_a >> MP_DIGIT_BIT;
      checksum_should_be_a = (checksum_should_be_a & MP_MASK) + carry;
   }
   if (checksum_should_be_a == MP_MASK) {
      checksum_should_be_a = 0;
   }

   if (checksum_a != checksum_should_be_a) {
      return true;
   }

   return false;
}


#endif
