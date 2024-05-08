#include "tommath_private.h"
#ifdef MP_PRIME_FERMAT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* performs one Fermat test.
 *
 * If "a" were prime then b**a == b (mod a) since the order of
 * the multiplicative sub-group would be phi(a) = a-1.  That means
 * it would be the same as b**(a mod (a-1)) == b**1 == b (mod a).
 *
 * Sets result to 1 if the congruence holds, or zero otherwise.
 */
mp_err mp_prime_fermat(const mp_int *a, const mp_int *b, bool *result)
{
   mp_int  t;
   mp_err  err = MP_OKAY;

   /* ensure b > 1 */
   if (mp_cmp_d(b, 1uL) != MP_GT) {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   /* init t */
   if ((err = mp_init(&t)) != MP_OKAY)                                   MP_TRACE_ERROR(err, LTM_ERR);

   /* compute t = b**a mod a */
   if ((err = mp_exptmod(b, a, a, &t)) != MP_OKAY)                       MP_TRACE_ERROR(err, LTM_ERR_1);

   /* is it equal to b? */
   *result = mp_cmp(&t, b) == MP_EQ;

LTM_ERR_1:
   mp_clear(&t);
LTM_ERR:
   return err;
}
#endif
