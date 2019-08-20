#include "tommath_private.h"
#ifdef BN_MP_TO_DECIMAL_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* stores a bignum as a decimal ASCII string, using Barrett
 * reduction if available.
 */

mp_err mp_to_decimal(const mp_int *a, char *str, size_t maxlen)
{
   mp_err err;

   if (MP_HAS(S_MP_TO_DECIMAL_FAST) && (a->used > 10)) {
      err = s_mp_to_decimal_fast(a, str, maxlen);
   } else {
      err = mp_to_radix(a, str, maxlen, 10);
   }

   return err;
}

#endif
