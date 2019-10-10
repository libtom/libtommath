#include "tommath_private.h"
#ifdef BN_MP_RADIX_SIZE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* returns size of ASCII representation */
mp_err mp_radix_size(const mp_int *a, int radix, int *size)
{
   mp_err err;
   mp_int a_, b;

   /* make sure the radix is in range */
   if ((radix < 2) || (radix > 64)) {
      return MP_VAL;
   }

   if (MP_IS_ZERO(a)) {
      *size = 2;
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
