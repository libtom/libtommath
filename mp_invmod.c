#include "tommath_private.h"
#ifdef MP_INVMOD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* hac 14.61, pp608 */
mp_err mp_invmod(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_err err = MP_OKAY;
   /* for all n in N and n > 0, n = 0 mod 1 */
   if (!mp_isneg(a) && mp_cmp_d(b, 1uL) == MP_EQ) {
      mp_zero(c);
      return MP_OKAY;
   }

   /* b cannot be negative and has to be >1 */
   if (mp_isneg(b) || (mp_cmp_d(b, 1uL) != MP_GT)) {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   /* if the modulus is odd we can use a faster routine instead */
   if (MP_HAS(S_MP_INVMOD_ODD) && mp_isodd(b)) {
      if ((err = s_mp_invmod_odd(a, b, c)) != MP_OKAY)                  MP_TRACE_ERROR(err, LTM_ERR);
      return err;
   }

   if (MP_HAS(S_MP_INVMOD)) {
      if ((err = s_mp_invmod(a, b, c)) != MP_OKAY)                      MP_TRACE_ERROR(err, LTM_ERR);
   } else {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

LTM_ERR:
   return err;

}
#endif
