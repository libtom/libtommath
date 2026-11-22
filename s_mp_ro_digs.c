#include "tommath_private.h"
#ifdef S_MP_RO_DIGS_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/* set a pointer and a length ionstead of a partial deep copy */
mp_err s_mp_ro_digs(const mp_int *src, int offset, int len, mp_int *c)
{
   mp_err err = MP_OKAY;
   int avail;

   if ((offset >= src->used) || (len <= 0)) {
      return MP_VAL;
   }

   avail = src->used - offset;

   if (len > avail) {
      len = avail;
   }

   while ((len > 0) && (src->dp[offset + len - 1] == 0)) {
      len--;
   }

   c->dp = src->dp + offset;
   c->used = len;
   c->alloc = len;
   c->sign = MP_ZPOS;

   return err;
}

#endif
