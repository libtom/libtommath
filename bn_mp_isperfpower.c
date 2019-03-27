#include "tommath_private.h"
#ifdef BN_MP_ISPERFPOWER_C
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



/* Test "z" if it is a prime power, that is both "p" and "b" are prime in z = p^b  */
#ifdef LTM_USE_EXTRA_FUNCTIONS
/*
   You might check out if rootout and exponent != 0 even in the case of result = MP_NO,
   this function output the intermediary values if it is a normal power z = a^b, tested
   with mp_ispower. The exponent wil be fully reduced: "b" will be prime. Useful for
   factorization.

   The limit for the individual factors depends on the quality of the pseudo-prime test.
   LibTomMath uses BPSW which has been tested up to 2^64 (~10^19)

   Relies on the sanity checks of mp_ispower, does not do any itself.
*/
int mp_isperfpower(const mp_int *z, int *result, mp_int *rootout, mp_int *exponent)
{
   mp_int new_exp, old_root;
   int e = MP_OKAY, prime, inner_result;

   /* Check if it is a power */
   if ((e = mp_ispower(z, result, rootout, exponent)) != MP_OKAY) {
      return e;
   }
   /* If not a power return output of mp_ispower */
   if (*result == MP_NO) {
      return e;
   }

   /* If power, check if root is a prime */
   if ((e = mp_prime_is_prime(rootout, 8, &prime)) != MP_OKAY) {
      return e;
   }
   /* If prime return output of mp_ispower */
   if (prime == MP_YES) {
      return e;
   }

   if ((e = mp_init_multi(&new_exp, &old_root, NULL)) != MP_OKAY) {
      return e;
   }

   mp_set(&new_exp, 1uL);
   for (;;) {
      if ((e = mp_mul(exponent, &new_exp, &new_exp)) != MP_OKAY) {
         goto LTM_ERR;
      }
      /* keep old root */
      if ((e = mp_copy(rootout, &old_root)) != MP_OKAY) {
         goto LTM_ERR;
      }
      /* feed mp_ispower with old_root */
      if ((e = mp_ispower(rootout, &inner_result, rootout, exponent)) != MP_OKAY) {
         goto LTM_ERR;
      }
      /* Are we at the root of the tree? */
      if (inner_result == MP_NO) {
         /* It is highly likely that it is a composite, so check */
         if ((e = mp_prime_is_prime(&old_root, 8, &prime)) != MP_OKAY) {
            goto LTM_ERR;
         }
         /* If the root is prime z is a perfect prime power
            and the exponent is in new_exp */
         if (prime == MP_YES) {
            *result = MP_YES;
            break;
         } else {
            *result = MP_NO;
            break;
         }
      }
   }

   /* Set output */
   mp_exch(&old_root, rootout);
   mp_exch(exponent, &new_exp);
LTM_ERR:
   /* Clean up */
   mp_clear_multi(&new_exp, &old_root, NULL);
   return e;
}
#endif
#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
