#include "tommath_private.h"
#ifdef BN_MP_INCR_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Increment "a" by one like "a++". Changes input! */
int mp_incr(mp_int *a)
{
   int e = MP_OKAY;
   if (MP_IS_ZERO(a)) {
      mp_set(a,1uL);
      return MP_OKAY;
   } else if (a->sign == MP_NEG) {
      a->sign = MP_ZPOS;
      if ((e = mp_decr(a)) != MP_OKAY) {
         return e;
      }
      /* There is no -0 in LTM */
      if (!MP_IS_ZERO(a)) {
         a->sign = MP_NEG;
      }
      return MP_OKAY;
   } else if (a->dp[0] < MP_MASK) {
      a->dp[0]++;
      return MP_OKAY;
   } else {
      return mp_add_d(a, 1uL,a);
   }
}
#endif
