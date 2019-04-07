#include "tommath_private.h"
#ifdef BN_MP_FWRITE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifndef LTM_NO_FILE
int mp_fwrite(const mp_int *a, int radix, FILE *stream)
{
   char *buf;
   int err, len, x;

   if ((err = mp_radix_size(a, radix, &len)) != MP_OKAY) {
      return err;
   }

   buf = (char *) XMALLOC((size_t)len);
   if (buf == NULL) {
      return MP_MEM;
   }

   if ((err = mp_toradix(a, buf, radix)) != MP_OKAY) {
      XFREE(buf, len);
      return err;
   }

   for (x = 0; x < len; x++) {
      if (fputc((int)buf[x], stream) == EOF) {
         XFREE(buf, len);
         return MP_VAL;
      }
   }

   XFREE(buf, len);
   return MP_OKAY;
}
#endif

#endif
