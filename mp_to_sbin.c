#include "tommath_private.h"
#ifdef MP_TO_SBIN_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* store in signed [big endian] format */
mp_err mp_to_sbin(const mp_int *a, uint8_t *buf, size_t maxlen, size_t *written)
{
   mp_err err = MP_OKAY;

   if (maxlen == 0u) {
      err = MP_BUF;
      MP_TRACE_ERROR(err, LTM_ERR);
   }
   if ((err = mp_to_ubin(a, buf + 1, maxlen - 1u, written)) != MP_OKAY)  MP_TRACE_ERROR(err, LTM_ERR);
   if (written != NULL) {
      (*written)++;
   }
   buf[0] = mp_isneg(a) ? (uint8_t)1 : (uint8_t)0;

LTM_ERR:
   return err;
}
#endif
