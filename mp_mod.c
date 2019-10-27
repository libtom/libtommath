#include "tommath_private.h"
#ifdef MP_MOD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* c = a mod b, 0 <= c < b if b > 0, b < c <= 0 if b < 0 */
mp_err mp_mod(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_int  t;
   mp_err  err;

   if ((err = mp_init_size(&t, b->used)) != MP_OKAY) {
      return err;
   }

   if ((err = mp_div(a, b, NULL, &t)) != MP_OKAY) {
      goto LBL_ERR;
   }

   if (mp_iszero(&t) || (t.sign == b->sign)) {
      err = MP_OKAY;
      mp_exch(&t, c);
   } else {
      err = mp_add(b, &t, c);
   }

LBL_ERR:
   mp_clear(&t);
   return err;
}
#endif
