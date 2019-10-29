#include "tommath_private.h"
#ifdef MP_FROM_SBIN_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* read signed bin, big endian, first byte is 0==positive or 1==negative */
mp_err mp_from_sbin(mp_int *a, const uint8_t *buf, size_t size)
{
   mp_err err;

   /* read magnitude */
   if ((err = mp_from_ubin(a, buf + 1, size - 1u)) != MP_OKAY) {
      return err;
   }

   /* first byte is 0 for positive, non-zero for negative */
   if (buf[0] == (uint8_t)0) {
      a->sign = MP_ZPOS;
   } else {
      a->sign = MP_NEG;
   }

   return MP_OKAY;
}
#endif
