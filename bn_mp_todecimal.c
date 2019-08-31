#include "tommath_private.h"
#ifdef BN_MP_TODECIMAL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* stores a bignum as a decimal ASCII string, using Barrett
 * reduction if available.
 */

mp_err mp_todecimal(const mp_int *a, char *str)
{
   mp_err err;

   if (MP_HAS(S_MP_TODECIMAL_FAST)) {
      err = s_mp_todecimal_fast(a, str);
   } else {
      err = mp_toradix(a, str, 10);
   }

   return err;
}

#endif
