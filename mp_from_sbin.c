#include "tommath_private.h"
#ifdef MP_FROM_SBIN_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* read signed bin, big endian, first byte is 0==positive or 1==negative */
mp_err mp_from_sbin(mp_int *a, const unsigned char *buf, size_t size)
{
   mp_err err;

   /* read magnitude */
   if ((err = mp_from_ubin(a, buf + 1, size - 1u)) != MP_OKAY) {
      return err;
   }

   /* first byte is 0 for positive, non-zero for negative */
   a->sign = (buf[0] == (unsigned char)0) ? MP_ZPOS : MP_NEG;

   return MP_OKAY;
}
#endif
