#include "tommath_private.h"
#ifdef S_MP_PRIME_IS_DIVISIBLE_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* determines if an integers is divisible by one
 * of the first PRIME_SIZE primes or not
 *
 * sets result to 0 if not, 1 if yes
 */
mp_err s_mp_prime_is_divisible(const mp_int *a, bool *result)
{
   int i;
   mp_err err = MP_OKAY;
   for (i = 0; i < MP_PRIME_TAB_SIZE; i++) {
      /* what is a mod LBL_prime_tab[i] */
      mp_digit res;
      if ((err = mp_mod_d(a, s_mp_prime_tab[i], &res)) != MP_OKAY)       MP_TRACE_ERROR(err, LTM_ERR);

      /* is the residue zero? */
      if (res == 0u) {
         *result = true;
         return err;
      }
   }

   /* default to not */
   *result = false;

LTM_ERR:
   return err;
}
#endif
