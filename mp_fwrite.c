#include "tommath_private.h"
#ifdef MP_FWRITE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifndef MP_NO_FILE
mp_err mp_fwrite(const mp_int *a, int radix, FILE *stream)
{
   char *buf;
   mp_err err;
   size_t len, written;

   if (MP_HAS(MP_RADIX_SIZE_OVERESTIMATE)) {
      if ((err = mp_radix_size_overestimate(a, radix, &len)) != MP_OKAY) {
         return err;
      }
   } else {
      if ((err = mp_radix_size(a, radix, &len)) != MP_OKAY) {
         return err;
      }
   }

   buf = (char *) MP_MALLOC(len);
   if (buf == NULL) {
      return MP_MEM;
   }

   if ((err = mp_to_radix(a, buf, len, &written, radix)) != MP_OKAY) {
      goto LBL_ERR;
   }

   if (fwrite(buf, written, 1uL, stream) != 1uL) {
      err = MP_ERR;
      goto LBL_ERR;
   }
   err = MP_OKAY;


LBL_ERR:
   MP_FREE_BUFFER(buf, len);
   return err;
}
#endif

#endif
