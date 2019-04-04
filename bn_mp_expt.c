#include "tommath_private.h"
#ifdef BN_MP_EXPT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* exponentiate a^b = c with a, b, c big integers */
int mp_expt(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_int tmp,e;
   int err;

   /* only positive exponents for now */
   if (b->sign == MP_NEG) {
      return MP_VAL;
   }

   if ((err = mp_init_multi(&tmp,&e,NULL)) != MP_OKAY) {
      return err;
   }
   if ((err = mp_copy(a,&tmp)) != MP_OKAY) {
      return err;
   }
   if ((err = mp_copy(b,&e)) != MP_OKAY) {
      return err;
   }
   if ((err = mp_set_int(c,1)) != MP_OKAY) {
      return err;
   }
   while (!IS_ZERO(&e)) {
      if (mp_isodd(&e)) {
         if ((err = mp_mul(c,&tmp,c)) != MP_OKAY) {
            return err;
         }
      }
      if ((err = mp_div_2(&e, &e)) != MP_OKAY) {
         return err;
      }
      if (!IS_ZERO(&e)) {
         if ((err = mp_sqr(&tmp,&tmp)) != MP_OKAY) {
            return err;
         }
      }
   }
   if (a->sign == MP_NEG) {
      c->sign = (mp_isodd(b))?MP_NEG:MP_ZPOS;
   }
   mp_clear_multi(&tmp,&e,NULL);
   return MP_OKAY;
}

#endif
