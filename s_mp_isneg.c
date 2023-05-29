#include "tommath_private.h"
#ifdef S_MP_ISNEG_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* We handle both, mp_int and mp_digit in the same function and mp_digit is unsigned */
bool s_mp_isneg(const mp_int *a)
{
   if (a != NULL) {
      return mp_isneg(a);
   }
   return false;
}

#endif
