#include "tommath_private.h"
#ifdef BN_MP_RADIX_SIZE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* returns size of ASCII representation */
mp_err mp_radix_size(const mp_int *a, int radix, int *size)
{
   mp_err err;
   mp_int a_, b;

   if (size == NULL) {
      return MP_VAL;
   } else {
      *size = 0;
   }

   /* make sure the radix is in range */
   if ((radix < 2) || (radix > 64)) {
      return MP_VAL;
   }

   if (MP_IS_ZERO(a)) {
      *size = 2;
      return MP_OKAY;
   }

   /* A small shortcut for powers of two. */
   if (!(radix&(radix-1))) {
      unsigned int x = (unsigned int)radix;
      int y, bit_count, rem;
      for (y=0; (y < 7) && !(x & 1u); y++) {
         x >>= 1;
      }
      bit_count = mp_count_bits(a);
      *size = bit_count/y;
      rem = bit_count - ((*size) * y);
      /* Add 1 for the remainder if any and 1 for "\0". */
      *size += ((rem == 0) ? 1 : 2) + (a->sign == MP_NEG);
      return MP_OKAY;
   }

   if ((err = mp_init(&b)) != MP_OKAY) {
      goto LBL_ERR;
   }

   a_ = *a;
   a_.sign = MP_ZPOS;
   if ((err = mp_ilogb(&a_, (uint32_t)radix, &b)) != MP_OKAY) {
      goto LBL_ERR;
   }

   *size = (int)mp_get_l(&b);

   /* mp_ilogb truncates to zero, hence we need one extra put on top and one for `\0`. */
   *size += 2 + (a->sign == MP_NEG);

LBL_ERR:
   mp_clear(&b);
   return err;
}

#endif
