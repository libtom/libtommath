#include "tommath_private.h"
#ifdef MP_FWRITE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#ifndef MP_NO_FILE
mp_err mp_fwrite(const mp_int *a, int radix, FILE *stream)
{
   char *buf;
   mp_err err = MP_OKAY;
   size_t size, written;

   if ((err = mp_radix_size_overestimate(a, radix, &size)) != MP_OKAY)   MP_TRACE_ERROR(err, LTM_ERR);

   buf = (char *) MP_MALLOC(size);
   if (buf == NULL)                                                      MP_TRACE_ERROR(MP_MEM, LTM_ERR);

   if ((err = mp_to_radix(a, buf, size, &written, radix)) == MP_OKAY) {
      written--;
      if (fwrite(buf, written, 1uL, stream) != 1uL) {
         err = MP_ERR;
         MP_TRACE_ERROR(err, LTM_ERR_FREE);
      }
      /* For the stacktracing */
   } else                                                                MP_TRACE_ERROR(err, LTM_ERR_FREE);

LTM_ERR_FREE:
   MP_FREE_BUF(buf, size);
LTM_ERR:
   return err;
}
#endif

#endif
