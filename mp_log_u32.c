#include "tommath_private.h"
#ifdef MP_LOG_U32_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

mp_err mp_log_u32(const mp_int *a, uint32_t base, uint32_t *c)
{
   if (a->sign == MP_NEG) {
      return MP_VAL;
   }

   if (MP_IS_ZERO(a)) {
      return MP_VAL;
   }

   if (base < 2u) {
      return MP_VAL;
   }

   if (MP_HAS(S_MP_LOG_POW2) && ((base & (base - 1u)) == 0u)) {
      *c = s_mp_log_pow2(a, base);
      return MP_OKAY;
   }

   if (MP_HAS(S_MP_LOG_D) && (a->used == 1)) {
      *c = (uint32_t)s_mp_log_d(base, a->dp[0]);
      return MP_OKAY;
   }

   if (MP_HAS(S_MP_LOG)) {
      return s_mp_log(a, base, c);
   }

   return MP_VAL;
}

#endif
