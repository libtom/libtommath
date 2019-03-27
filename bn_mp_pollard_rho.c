#include "tommath_private.h"
#ifdef BN_MP_POLLARD_RHO_C
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



/* Find one factor of a positive integer using the Pollard-Rho algorithm */

#ifdef LTM_USE_EXTRA_FUNCTIONS
/* a small helper to keep the code readable */
static int s_square_mod_add(const mp_int *y, const mp_int *c, const mp_int *n, mp_int *r)
{
   int e = MP_OKAY;
   if ((e = mp_sqrmod(y, n, r)) != MP_OKAY) {
      return e;
   }
   if ((e = mp_add(r, c, r)) != MP_OKAY) {
      return e;
   }
   if ((e = mp_mod(r, n, r)) != MP_OKAY) {
      return e;
   }
   return e;
}
/*
   This implementation uses Richar P. Brent's refinements as described in
   http://wwwmaths.anu.edu.au/~brent/pd/rpb051i.pdf section 7.

   Good enough for factors up to ~40 bits.

   Does not check anything about the input, just if it is one or even.

   Minimum preparation before use:
     - check if "n" is a (pseudo) prime
     - check if "n" is a square

   Recommended preparation:
     - check if "n" is (pseudo) prime. LibTomMath uses BPSW wich is tested
       up to 2^64 (~10^19).
     - check if "n" is a power (n = a^b where "a" can be a power, too,
       so check recursively)
     - check if "n" is a prime power (n = p^b with "p" prime)
     - trial division up to the architecture dependent limit. If you
       are using mp_next_small_prime() you can try the first 6542
       primes, even with MP_8BIT. Output "n_t"
     - check if "n_t" is (pseudo) prime. LibTomMath uses BPSW wich is tested
       up to 2^64 (~10^19).
     - check if "n_t" is a power (n = a^b where "a" can be a power, too,
       so check recursively)
     - check if "n_t" is a prime power (n = p^b with "p" prime)

*/
int mp_pollard_rho(const mp_int *n, mp_int *factor)
{
   mp_int x, y, ys, c, tmp;
   mp_int d, q;
   /*
      {j,r} might get problematic if sizeof(long) <= sizeof(short),
      15 bit might not be enough.
    */
   long i, j, m, r;
   int e = MP_OKAY, ilog2, ilog2_rand, digits;

   if (LTM_IS_UNITY(n)) {
      if ((e = mp_copy(n, factor)) != MP_OKAY) {
         return e;
      }
      return MP_OKAY;
   }
   if (IS_EVEN(n)) {
      mp_set(factor, 2uL);
      return MP_OKAY;
   }

   if ((e = mp_init_multi(&x, &y, &ys, &c, &d, &q, &tmp, NULL)) != MP_OKAY) {
      return e;
   }
   ilog2 = mp_count_bits(n);
   digits = n->used;

   if ((e = mp_rand(&y, digits)) != MP_OKAY) {
      goto LTM_END;
   }
   ilog2_rand = mp_count_bits(&y);
   if (ilog2_rand >= ilog2) {
      if ((e = mp_div_2d(&y, ilog2_rand - (ilog2 + 1), &y,NULL)) != MP_OKAY) {
         goto LTM_END;
      }
   }

   if ((e = mp_copy(&y, &ys)) != MP_OKAY) {
      goto LTM_END;
   }

   /* 0 < c < (n-2) */
   if ((e = mp_rand(&c, digits)) != MP_OKAY) {
      goto LTM_END;
   }
   ilog2_rand = mp_count_bits(&c);
   if (ilog2_rand >= ilog2) {
      if ((e = mp_div_2d(&c, ilog2_rand - (ilog2 + 1), &c,NULL)) != MP_OKAY) {
         goto LTM_END;
      }
   }
   if (mp_cmp_d(&c,3uL) != MP_LT) {
      if ((e = mp_sub_d(&c, 2uL, &c)) != MP_OKAY) {
         goto LTM_END;
      }
   }

   m = 1000L;
   r = 1;
   mp_set(&d, 1uL);
   mp_set(&q, 1uL);

   do {
      if ((e = mp_copy(&y, &x)) != MP_OKAY) {
         goto LTM_END;
      }
      for (i = 1; i <= r; i++) {
         s_square_mod_add(&y, &c, n, &y);
      }
      j = 0;
      do {
         mp_copy(&y, &ys);
         for (i = 1; i <= MIN(m, r - j); i++) {
            if ((e = s_square_mod_add(&y, &c, n, &y)) != MP_OKAY) {
               goto LTM_END;
            }
            if ((e = mp_sub(&x, &y, &tmp)) != MP_OKAY) {
               goto LTM_END;
            }
            tmp.sign = MP_ZPOS;
            if ((e = mp_mulmod(&q, &tmp, n, &q)) != MP_OKAY) {
               goto LTM_END;
            }
         }
         if ((e = mp_gcd(&q, n, &d)) != MP_OKAY) {
            goto LTM_END;
         }
         /* TODO: check for overflow */
         j += m;
      } while ((j < r) && LTM_IS_UNITY(&d));
      /* TODO: check for overflow */
      r *= 2;
   } while (LTM_IS_UNITY(&d));
   if (mp_cmp(n, &d) == MP_EQ) {
      do {
         if ((e = s_square_mod_add(&ys, &c, n, &ys)) != MP_OKAY) {
            goto LTM_END;
         }
         if ((e = mp_sub(&x, &ys, &tmp)) != MP_OKAY) {
            goto LTM_END;
         }
         tmp.sign = MP_ZPOS;
         if ((e = mp_gcd(&tmp, n, &d)) != MP_OKAY) {
            goto LTM_END;
         }
      } while (LTM_IS_UNITY(&d));
   }
LTM_END:
   mp_exch(&d, factor);
   mp_clear_multi(&x, &y, &ys, &c, &d, &q, &tmp, NULL);
   return e;
}
#endif
#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
