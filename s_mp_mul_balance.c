#include "tommath_private.h"
#ifdef S_MP_MUL_BALANCE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* single-digit multiplication with the smaller number as the single-digit */
mp_err s_mp_mul_balance(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_int a0, tmp, r;
   mp_err err = MP_OKAY;
   int i, j,
       nblocks = MP_MAX(a->used, b->used) / MP_MIN(a->used, b->used),
       bsize = MP_MIN(a->used, b->used);

   if ((err = mp_init_size(&a0, bsize + 2)) != MP_OKAY)                  MP_TRACE_ERROR(err, LTM_ERR);
   if ((err = mp_init_multi(&tmp, &r, NULL)) != MP_OKAY) {
      mp_clear(&a0);
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   /* Make sure that A is the larger one*/
   if (a->used < b->used) {
      MP_EXCH(const mp_int *, a, b);
   }

   for (i = 0, j=0; i < nblocks; i++) {
      /* Cut a slice off of a */
      a0.used = bsize;
      s_mp_copy_digs(a0.dp, a->dp + j, a0.used);
      j += a0.used;
      mp_clamp(&a0);

      /* Multiply with b */
      if ((err = mp_mul(&a0, b, &tmp)) != MP_OKAY)                       MP_TRACE_ERROR(err, LTM_ERR_1);
      /* Shift tmp to the correct position */
      if ((err = mp_lshd(&tmp, bsize * i)) != MP_OKAY)                   MP_TRACE_ERROR(err, LTM_ERR_1);
      /* Add to output. No carry needed */
      if ((err = mp_add(&r, &tmp, &r)) != MP_OKAY)                       MP_TRACE_ERROR(err, LTM_ERR_1);
   }
   /* The left-overs; there are always left-overs */
   if (j < a->used) {
      a0.used = a->used - j;
      s_mp_copy_digs(a0.dp, a->dp + j, a0.used);
      j += a0.used;
      mp_clamp(&a0);

      if ((err = mp_mul(&a0, b, &tmp)) != MP_OKAY)                       MP_TRACE_ERROR(err, LTM_ERR_1);
      if ((err = mp_lshd(&tmp, bsize * i)) != MP_OKAY)                   MP_TRACE_ERROR(err, LTM_ERR_1);
      if ((err = mp_add(&r, &tmp, &r)) != MP_OKAY)                       MP_TRACE_ERROR(err, LTM_ERR_1);
   }

   mp_exch(&r,c);
LTM_ERR_1:
   mp_clear_multi(&a0, &tmp, &r,NULL);
LTM_ERR:
   return err;
}
#endif
