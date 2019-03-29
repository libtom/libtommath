#include "tommath_private.h"
#ifdef BN_MP_MILLER_BACH_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * SPDX-License-Identifier: Unlicense
 */

/*
   If "limit" is set to 2log(z)^2 and assuming the generalized Riemann hypothesis
   this function is correct but slow, very slow for large primes.

   Primes generated with mp_prime_random_ex(&z, 8, size, LTM_PRIME_SAFE, myrng, NULL)
   (myrng() reads from /dev/urandom) and checked with PariGP's isprime()

    2^n  testing-time   time to generate
    128    0m00.061s       0m00.176s
    256    0m00.490s       0m00.788s
    512    0m06.233s       0m05.257s
    768    0m34.974s       0m02.916s
   1024    2m12.393s       0m45.018s
   1536   11m39.058s       2m25.890s
   2048   45m13.408s       2m57.433s

   To get something to compare with: Pari/GP needed ca 4 seconds for 2^1024 with a proof.
*/
#ifdef LTM_USE_EXTRA_FUNCTIONS
/* Run Miller-Rabin tests with all primes up to "limit" as witnesses against "z" */
int mp_miller_bach(const mp_int *z, LTM_SIEVE_UINT limit, int *result)
{
   mp_sieve sieve;
   LTM_SIEVE_UINT k, ret;
   mp_int b;
   int r;
   int e;

   if (IS_ZERO(z) || (limit < 2) || (z->sign == MP_NEG)) {
      return MP_VAL;
   }

   if( (e = mp_init(&b) ) != MP_OKAY){
      *result = MP_NO;
      return e;
   }

   mp_sieve_init(&sieve);

   for (k = 0, ret = 0; ret < (LTM_SIEVE_UINT)limit; k = ret) {
      if ((e = mp_next_small_prime(k + 1, &ret, &sieve)) != MP_OKAY) {
         if (e == LTM_SIEVE_MAX_REACHED) {
            if ((e = mp_set_long(&b, (unsigned long)ret)) != MP_OKAY) {
               goto LTM_ERR;
            }
            if ((e = mp_prime_miller_rabin(z, &b, &r)) != MP_OKAY) {
               goto LTM_ERR;
            }
            e = MP_VAL;
            goto LTM_END;
         }
         goto LTM_ERR;
      }
      if (ret <= (LTM_SIEVE_UINT)limit) {
         if ((e = mp_set_long(&b,(unsigned long)ret)) != MP_OKAY) {
            goto LTM_ERR;
         }
         if ((e = mp_prime_miller_rabin(z, &b, &r)) != MP_OKAY) {
            goto LTM_ERR;
         }
         if (r == MP_NO) {
            break;
         }
      }
   }

LTM_END:
   *result = r;
   mp_clear(&b);
   mp_sieve_clear(&sieve);
   return e;
LTM_ERR:
   mp_sieve_clear(&sieve);
   mp_clear(&b);
   return e;
}
#endif

#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
