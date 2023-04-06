#include "tommath_private.h"
#ifdef MP_PREC_SMALL_PRIME_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */


/*
 * Mimics behaviour of Pari/GP's precprime(n)
 * If n is prime set *result to n else set *result to first prime < n
 * and 0 in case of error
 */
mp_err mp_prec_small_prime(mp_sieve_prime n, mp_sieve_prime *result, mp_sieve *sieve)
{
   bool ret = false;
   mp_err e = MP_OKAY;

   if (n == 2lu) {
      *result = 2lu;
      return e;
   }

   if (n < 2lu) {
      *result = 0lu;
      return e;
   }

   for (; ret == false; n--) {
      if ((e = mp_is_small_prime(n, &ret, sieve)) != MP_OKAY) {
         *result = 0lu;
         return e;
      }
   }
   *result = n + 1lu;

   return e;
}

#endif
