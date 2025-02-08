#include "tommath_private.h"
#ifdef MP_INIT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* init a new mp_int */
mp_err mp_init(mp_int *a)
{
   mp_err err = MP_OKAY;

   /* allocate memory required and clear it */
   a->dp = (mp_digit *) MP_CALLOC((size_t)MP_DEFAULT_DIGIT_COUNT, sizeof(mp_digit));
   if (a->dp == NULL) {
      err = MP_MEM;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   /* set the used to zero, allocated digits to the default precision
    * and sign to positive */
   a->used  = 0;
   a->alloc = MP_DEFAULT_DIGIT_COUNT;
   a->sign  = MP_ZPOS;

LTM_ERR:
   return err;
}
#endif
