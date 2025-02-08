#include "tommath_private.h"
#ifdef MP_INIT_SIZE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* init an mp_init for a given size */
mp_err mp_init_size(mp_int *a, int size)
{
   mp_err err = MP_OKAY;
   if (size < 0) {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   size = MP_MAX(MP_MIN_DIGIT_COUNT, size);

   if (size > MP_MAX_DIGIT_COUNT) {
      err = MP_OVF;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   /* alloc mem */
   a->dp = (mp_digit *) MP_CALLOC((size_t)size, sizeof(mp_digit));
   if (a->dp == NULL) {
      err = MP_MEM;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   /* set the members */
   a->used  = 0;
   a->alloc = size;
   a->sign  = MP_ZPOS;

LTM_ERR:
   return err;
}
#endif
