#include "tommath_private.h"
#ifdef MP_RADIX_SIZE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* returns size of ASCII representation */
mp_err mp_radix_size(const mp_int *a, int radix, size_t *size)
{
   mp_err err;
   mp_int a_;
   uint32_t b;

   /* make sure the radix is in range */
   if ((radix < 2) || (radix > 64)) {
      return MP_VAL;
   }

   if (MP_IS_ZERO(a)) {
      *size = 2;
      return MP_OKAY;
   }

   a_ = *a;
   a_.sign = MP_ZPOS;
   if ((err = mp_log_u32(&a_, (uint32_t)radix, &b)) != MP_OKAY) {
      goto LBL_ERR;
   }

   /* mp_ilogb truncates to zero, hence we need one extra put on top and one for `\0`. */
   *size = (size_t)b + 2U + ((a->sign == MP_NEG) ? 1U : 0U);

LBL_ERR:
   return err;
}
#endif
