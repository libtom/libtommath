#include "tommath_private.h"
#ifdef S_MP_IS_SQR_WRONG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



bool s_mp_is_sqr_wrong(const mp_int *a, const mp_int *c)
{
   mp_word checksum_a, checksum_c, checksum_sqr, carry;
   checksum_a = s_mp_compute_limb_checksum(a);
   checksum_c = s_mp_compute_limb_checksum(c);

   checksum_sqr = checksum_a * checksum_a;

   while (checksum_sqr > MP_MASK) {
      carry = checksum_sqr >> MP_DIGIT_BIT;
      checksum_sqr = (checksum_sqr & MP_MASK) + carry;
   }
   if (checksum_sqr == MP_MASK) {
      checksum_sqr = 0u;
   }
   if (checksum_sqr != checksum_c) {
      return false;
   }
   return true;
}


#endif
