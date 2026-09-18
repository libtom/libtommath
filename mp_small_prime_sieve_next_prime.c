#include "tommath_private.h"
#ifdef MP_SMALL_PRIME_SIEVE_NEXT_PRIME_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */




mp_err mp_small_prime_sieve_next_prime(ERAT_UINT n, ERAT_UINT *result, mp_erat_sieve *sieve)
{
   bool ret = false;
   mp_err err = MP_OKAY;

   /* Shortcut */
   if (n < 2) {
      *result = 2;
      return err;
   }
   /* Even primes get a special treatment */
   if (n == 2) {
      *result = 3;
      return err;
   }
   /* Shortcut */
   if ((n % 2) == 0) {
      n++;
   }

   /* Check for upper limit */
   if (n == ERAT_BIGGEST_PRIME) {
      *result = ERAT_BIGGEST_PRIME;
      return err;
   }

   /* Shortcut */
   if (n > ERAT_BIGGEST_PRIME) {
      *result = 0;
      return MP_OVF;
   }

   /* Now we are down to the range 3 < n < ERAT_BIGGEST_PRIME with n odd */

   /* As long as the flag is false check the odd numbers >= n */
   for (; ret == false; n+=2) {
      /* just call is_small_prime(), it does all of the heavy work */
      if ((err = mp_small_prime_sieve_is_small_prime(n, &ret, sieve)) != MP_OKAY) {
         *result = 0;
         return err;
      }
   }
   /* The index overshots be two, subtract two unconditionally */
   *result = n - 2;
   return err;
}



#endif
