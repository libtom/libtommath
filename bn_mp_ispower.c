#include "tommath_private.h"
#ifdef BN_MP_ISPOWER_C
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

/* Test if "z" is a power, that is z = a^b with "b" prime but "a" might be composite */
#ifdef LTM_USE_EXTRA_FUNCTIONS
/* Mainly for readability but also changed such that highbit(2^x) == lowbit(2^x) == x */
#define LTM_HIGHBIT(x) (mp_count_bits((x)) - 1)
#define LTM_LOWBIT(x)  (mp_cnt_lsb((x)))

int mp_ispower(const mp_int *z, int *result, mp_int *rootout, mp_int *exponent)
{
   mp_sieve sieve;

   LTM_SIEVE_UINT k, r;

   mp_int root, power;

   unsigned long max, highbit, lowbit;
   int ret, e = MP_OKAY;

   mp_sieve_init(&sieve);

   /* No negative input (for now) */
   if (IS_ZERO(z) || (z->sign == MP_NEG)) {
      return MP_VAL;
   }

   max = (unsigned long) LTM_HIGHBIT(z);
   /*
      No need to check indices > log_2(n).
      The max number of runs is therefore pi(log_2(n)) which makes about
      67 runs for a hundert decimal digits large number.
    */
   highbit = max;
   lowbit = (unsigned long) LTM_LOWBIT(z);

   /* Is it N=2^p ? */
   if (highbit == lowbit) {
      if (exponent != NULL) {
         if ((e = mp_set_int(exponent, highbit)) != MP_OKAY) {
            return e;
         }
      }
      if (rootout != NULL) {
         mp_set(rootout, 2uL);
      }
      *result = MP_YES;
      return MP_OKAY;
   }

   if ((e = mp_is_square(z, &ret)) != MP_OKAY) {
      return e;
   }
   if (ret == MP_YES) {
      /* mp_is_square() does compute a square-root but does not return the result */
      if (rootout != NULL) {
         if ((e = mp_sqrt(z, rootout)) != MP_OKAY) {
            return e;
         }
      }
      if (exponent != NULL) {
         mp_set(exponent, 2uL);
      }
      *result = MP_YES;
      return MP_OKAY;
   }
   /* Can happen for e.g.: MP_8BIT (Use bigint for MP_8BIT instead?)*/
   if (sizeof(LTM_SIEVE_UINT) < sizeof(highbit)) {
      if (LTM_SIEVE_UINT_MAX < highbit) {
         return MP_VAL;
      }
   }

   if ((e = mp_init_multi(&root, &power, NULL)) != MP_OKAY) {
      return e;
   }

   for (k = 0, r = 0; r < (LTM_SIEVE_UINT)highbit; k = r) {
      if ((e = mp_next_small_prime(k + 1, &r, &sieve)) != MP_OKAY) {
         if (e == LTM_SIEVE_MAX_REACHED) {
            if ((e = mp_n_root(z, (mp_digit)(k), &root)) != MP_OKAY) {
               return e;
            }
            if ((e = mp_expt_d(&root, (mp_digit)(k), &power)) != MP_OKAY) {
               return e;
            }
            if (mp_cmp(z, &power) == MP_EQ) {
               if (rootout != NULL) {
                  mp_exch(&root, rootout);
               }
               if (exponent != NULL) {
                  if ((e = mp_set_int(exponent, (mp_digit)(k))) != MP_OKAY) {
                     return e;
                  }
               }
               *result = MP_YES;
               goto LTM_END;
            }
            break;
         }
         goto LTM_END;
      }
      if ((r <= max) && (k != 0uL)) {
         if ((e = mp_n_root(z, (mp_digit)(k), &root)) != MP_OKAY) {
            return e;
         }
         if ((e = mp_expt_d(&root, (mp_digit)(k), &power)) != MP_OKAY) {
            return e;
         }
         if (mp_cmp(z, &power) == MP_EQ) {
            if (rootout != NULL) {
               mp_exch(&root, rootout);
            }
            if (exponent != NULL) {
               if ((e = mp_set_int(exponent, (mp_digit)(k))) != MP_OKAY) {
                  return e;
               }
            }
            *result = MP_YES;
            goto LTM_END;
         }
      }
   }
   if (rootout != NULL) {
      mp_set(rootout, 0uL);
   }
   if (exponent != NULL) {
      mp_set(exponent, 0uL);
   }
   *result = MP_NO;

LTM_END:
   mp_clear_multi(&root, &power,NULL);
   mp_sieve_clear(&sieve);
   return e;
}
#endif
#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
