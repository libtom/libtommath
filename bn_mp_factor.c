#include "tommath_private.h"
#ifdef BN_MP_FACTOR_C
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

#ifdef LTM_USE_EXTRA_FUNCTIONS
/* Factor integer. Good for up to about 35-40 bit large factors */
static int mp_factor_intern(mp_int *z, mp_factors *factors)
{

   int e = MP_OKAY;
   int all, ilog2;
   /* int perfpow; */

   mp_int factor, rootout, exponent, quot, rem, r;


   ilog2 = mp_count_bits(z);
   /* sizeof(int) < 4 is possible (e.g.: for MP_8BIT/MP_16BIT) */
   if (ilog2 < 10) {
      ilog2 = 10;
   } else if (ilog2 > (int)(2 * (sizeof(int) * CHAR_BIT) - 1u)) {
      ilog2 = (int)((2 * sizeof(int) * CHAR_BIT) - 1u);
   }

   /* floor(sqrt(z)) */
   ilog2 = (int)(1uL << (ilog2/2));

#ifdef MP_8BIT
   ilog2 = ((ilog2 > 0) && (ilog2 < (int)(LTM_SIEVE_UINT_MAX)))?ilog2:(int)(LTM_SIEVE_UINT_MAX);
#else
   ilog2 = ((ilog2 > 0) && (ilog2 < (int)(LTM_SIEVE_UINT_MAX_SQRT)))?ilog2:(int)(LTM_SIEVE_UINT_MAX_SQRT);
#endif

   if ((e = mp_init_multi(&factor, &rootout, &exponent, &quot, &rem, &r, NULL)) != MP_OKAY) {
      return e;
   }
   /* Trial division */
   if ((e = mp_trial(z, ilog2, factors, &r)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /* Check remainder to see if we are done here */
   if (mp_cmp_d(&r,1uL) == MP_EQ) {
      goto LTM_END;
   } else {
      /* Try to find more factors until the remainder is 1 (one)*/
      do {
         /*
            Check if the remainder is already prime.
            mp_prime_is_prime() uses BPSW which is tested up to 2^64 (~10^19)
            but that number is already quite large for a factor.
          */
         if ((e = mp_prime_is_prime(&r, 8, &all)) != MP_OKAY) {
            goto LTM_ERR;
         }

         if (all == MP_NO) {
#if 0
            if ((e = mp_isperfpower(&r, &perfpow, &rootout, &exponent)) != MP_OKAY) {
               goto LTM_ERR;
            }
            if (perfpow == MP_YES) {
               for (;;) {
                  if ((e = mp_factors_add(&rootout, factors)) != MP_OKAY) {
                     goto LTM_ERR;
                  }
                  if ((e = mp_decr(&exponent)) != MP_OKAY) {
                     goto LTM_ERR;
                  }
                  /* the root is prime, so: work done */
                  if (IS_ZERO(&exponent)) {
                     goto LTM_END;
                  }
               }
            }
#endif
            /*
              Pollard-Rho is good for up to 35 bit large factors, 40 bit if
              you are not too impatient.
            */
            if ((e = mp_pollard_rho(&r, &factor)) != MP_OKAY) {
               goto LTM_ERR;;
            }
            /* Pollard-Rho found a factor but it might not be prime */
            if ((e = mp_prime_is_prime(&factor, 8, &all)) != MP_OKAY) {
               goto LTM_ERR;
            }
            if (all == MP_YES) {
               do {
                  if ((e = mp_div(&r, &factor, &quot, &rem)) != MP_OKAY) {
                     goto LTM_ERR;
                  }
                  if (IS_ZERO(&rem)) {
                     if ((e = mp_factors_add(&factor, factors)) != MP_OKAY) {
                        goto LTM_ERR;
                     }
                     mp_exch(&r, &quot);
                  }
               } while (IS_ZERO(&rem));
            }
         } else {
            /* remainder is prime, end of story */
            if ((e = mp_factors_add(&r, factors)) != MP_OKAY) {
               goto LTM_ERR;
            }
            break;
         }
      } while (!LTM_IS_UNITY(&r));
   }

LTM_ERR:
LTM_END:
   mp_clear_multi(&factor, &rootout, &exponent, &quot, &rem, &r, NULL);
   return e;
}


int mp_factor(const mp_int *z, mp_factors *factors)
{
   int e = MP_OKAY;
   int is_prime;
   /*int is_perfect_power;*/

   mp_int tmp, z_;
   mp_int rootout, exponent;
   mp_int factor;
   mp_int divisor, quotient, remainder;

   /* No negative numbers here, caller needs to take care of them. */
   if (IS_ZERO(z) || (z->sign == MP_NEG)) {
      return MP_VAL;
   }
   /* 1 is not prime by definition */
   if (LTM_IS_UNITY(z)) {
      return MP_OKAY;
   }

   mp_factors_zero(factors);

   if (mp_cmp_d(z, 4) == MP_LT) {
      return mp_factors_add(z,factors);
   }

   if ((e = mp_init_multi(&tmp, &rootout, &exponent, &factor, &divisor, &quotient, &remainder, NULL)) != MP_OKAY) {
      return e;
   }
   if ((e = mp_copy(z, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   z_ = *z;

   /* TODO: check for size and use the deterministic M-R test instead, if the size if right */
   if ((e = mp_prime_is_prime(&z_, 8, &is_prime)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if (is_prime == MP_YES) {
      if ((e = mp_factors_add(z,factors)) != MP_OKAY) {
         goto LTM_ERR;
      }
      goto LTM_CHECK;
   }

   /* Switched off temporarily */
#if 0
   if ((e = mp_isperfpower(&z_, &is_perfect_power, &rootout, &exponent)) != MP_OKAY) {
      goto LTM_ERR;
   }

   /* It is a prime power */
   if (is_perfect_power == MP_YES) {
      for (;;) {
         if ((e = mp_factors_add(&rootout, factors)) != MP_OKAY) {
            goto LTM_ERR;
         }
         if ((e = mp_decr(&exponent)) != MP_OKAY) {
            goto LTM_ERR;
         }
         /* the root is prime, hence work is done here */
         if (IS_ZERO(&exponent)) {
            goto LTM_CHECK;
         }
      }
   }
#endif

   if ((e = mp_factor_intern(&z_, factors)) != MP_OKAY) {
      goto LTM_ERR;
   }


LTM_CHECK:
   if ((e = mp_factors_product(factors, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if (mp_cmp(&tmp, z) != MP_EQ) {
      e = MP_VAL;
      goto LTM_ERR;
   }
LTM_ERR:
   mp_clear_multi(&tmp, &rootout, &exponent, &factor, &divisor, &quotient, &remainder, NULL);
   return e;
}
#endif
#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
