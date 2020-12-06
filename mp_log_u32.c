#include "tommath_private.h"
#ifdef MP_LOG_U32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


mp_err mp_log_u32(const mp_int *a, uint32_t base, uint32_t *c)
{
   mp_int b;
   mp_err err;
   int cc;

   if ((err = mp_init_u32(&b, base)) != MP_OKAY)                                                          goto LTM_ERR;
   if ((err = mp_log(a, &b, &cc)) != MP_OKAY)                                                             goto LTM_ERR;
   *c = (uint32_t)cc;

LTM_ERR:
   mp_clear(&b);
   return err;
}


#endif
