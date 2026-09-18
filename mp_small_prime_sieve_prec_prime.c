#include "tommath_private.h"
#ifdef MP_SMALL_PRIME_SIEVE_PREC_PRIME_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



mp_err mp_small_prime_sieve_prec_prime(ERAT_UINT n, ERAT_UINT *result, mp_erat_sieve *sieve)
{
   bool ret = false;
   mp_err err = MP_OKAY;

   /* Shortcut */
   if (n < 2) {
      *result = 0;
      return err;
   }

   /* Even primes get a special treatment */
   if (n == 2) {
      *result = 2;
      return err;
   }

   /* The rest of the primes are odd */
   if ((n % 2) == 0) {
      n--;
   }

   /* Now we are down to the range 2 < n < ERAT_UINT_MAX */

   /* As long as the flag is false check the odd numbers <= n */
   for (; ret == false; n -= 2) {
      if ((err = mp_small_prime_sieve_is_small_prime(n, &ret, sieve)) != MP_OKAY) {
         *result = 0;
         return err;
      }
   }

   /* The index overshots be two, add two unconditionally */
   *result = n + 2;

   return err;
}



#endif
