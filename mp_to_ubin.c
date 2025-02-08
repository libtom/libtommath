#include "tommath_private.h"
#ifdef MP_TO_UBIN_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* store in unsigned [big endian] format */
mp_err mp_to_ubin(const mp_int *a, uint8_t *buf, size_t maxlen, size_t *written)
{
   size_t  x, count;
   mp_err  err = MP_OKAY;
   mp_int  t;

   count = mp_ubin_size(a);
   if (count > maxlen) {
      err = MP_BUF;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   if ((err = mp_init_copy(&t, a)) != MP_OKAY)                           MP_TRACE_ERROR(err, LTM_ERR);

   for (x = count; x --> 0u;) {
      buf[x] = (uint8_t)(t.dp[0] & 255u);
      if ((err = mp_div_2d(&t, 8, &t, NULL)) != MP_OKAY)                 MP_TRACE_ERROR(err, LTM_ERR_1);
   }

   if (written != NULL) {
      *written = count;
   }

LTM_ERR_1:
   mp_clear(&t);
LTM_ERR:
   return err;
}
#endif
