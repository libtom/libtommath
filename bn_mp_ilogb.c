#include "tommath_private.h"
#ifdef BN_MP_ILOGB_C
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
/* Compute log_{base}(a) */
static mp_word s_pow(mp_word base, mp_word exponent)
{
   mp_word result = 1uLL;
   while (exponent) {
      if ((exponent & 0x1) == 1) {
         result *= base;
      }
      exponent >>= 1;
      base *= base;
   }

   return result;
}

static mp_digit digit_ilogb(mp_digit base, mp_digit n)
{
   mp_word low = 0uLL, bracket_low = 1uLL, high = 1uLL, mid, bracket_mid;
   mp_word bracket_high, N;
   mp_digit ret;

   if (n < base) {
      return (mp_digit)0uL;
   }
   if (n == base) {
      return (mp_digit)1uL;
   }

   bracket_high = (mp_word) base ;
   N = (mp_word) n;

   while (bracket_high < N) {
      low = high;
      bracket_low = bracket_high;
      high <<= 1;
      bracket_high *= bracket_high;
   }

   while ((high - low) > 1uLL) {
      mid = (low + high) >> 1;
      bracket_mid = bracket_low * s_pow(base, mid - low) ;

      if (N < bracket_mid) {
         high = mid ;
         bracket_high = bracket_mid ;
      }
      if (N > bracket_mid) {
         low = mid ;
         bracket_low = bracket_mid ;
      }
      if (N == bracket_mid) {
         return (mp_digit) mid;
      }
   }

   if (bracket_high == N) {
      ret = (mp_digit) high;
   } else {
      ret = (mp_digit) low;
   }

   return ret;
}

/* TODO: output could be "int" because the output of mp_radix_size is int, too,
         as is the output of mp_bitcount.
         With the same problem: max size is INT_MAX * MP_DIGIT not INT_MAX only!
*/
int mp_ilogb(mp_int *a, mp_digit base, mp_int *c)
{
   int err, cmp;
   mp_digit high, low, mid;
   mp_int bracket_low, bracket_high, bracket_mid, t, bi_base;
   mp_digit tmp;

   err = MP_OKAY;
   if (a->sign == MP_NEG) {
      return MP_VAL;
   }
   if (IS_ZERO(a)) {
      return MP_VAL;
   }

   if (base < 2) {
      return MP_VAL;
   } else if (base == 2) {
      tmp = (mp_digit)mp_count_bits(a) - 1;
      mp_set(c, tmp);
      return err;
   } else if (a->used == 1) {
      tmp = digit_ilogb(base, a->dp[0]);
      mp_set(c, tmp);
      return err;
   }


   cmp = mp_cmp_d(a, base);

   if (cmp == MP_LT) {
      mp_zero(c);
      return err;
   } else if (cmp == MP_EQ) {
      mp_set(c, (mp_digit)1uL);
      return err;
   }

   if ((err =
           mp_init_multi(&bracket_low, &bracket_high,
                         &bracket_mid, &t, &bi_base, NULL)) != MP_OKAY) {
      return err;
   }

   low = 0uL;
   mp_set(&bracket_low, 1uL);
   high = 1uL;

   mp_set(&bracket_high, base);

   /*
       A kind of Giant-step/baby-step algorithm.
       Idea shamelessly stolen from https://programmingpraxis.com/2010/05/07/integer-logarithms/2/
       The effect is asymptotic, hence needs benchmarks to test if the Giant-step should be skipped
       for small n.
    */
   while (mp_cmp(&bracket_high, a) == MP_LT) {
      low = high;
      if ((err = mp_copy(&bracket_high, &bracket_low)) != MP_OKAY) {
         goto LBL_ERR;
      }
      high <<= 1;
      if ((err = mp_sqr(&bracket_high, &bracket_high)) != MP_OKAY) {
         goto LBL_ERR;
      }
   }
   mp_set(&bi_base, base);

   while ((high - low) > 1) {
      mid = (high + low) >> 1;
      if ((err = mp_expt_d(&bi_base, (mid - low), &t)) != MP_OKAY) {
         goto LBL_ERR;
      }
      if ((err = mp_mul(&bracket_low, &t, &bracket_mid)) != MP_OKAY) {
         goto LBL_ERR;
      }
      cmp = mp_cmp(a, &bracket_mid);
      if (cmp == MP_LT) {
         high = mid;
         mp_exch(&bracket_mid, &bracket_high);
      }
      if (cmp == MP_GT) {
         low = mid;
         mp_exch(&bracket_mid, &bracket_low);
      }
      if (cmp == MP_EQ) {
         mp_set(c, mid);
         goto LBL_ERR;
      }
   }

   if (mp_cmp(&bracket_high, a) == MP_EQ) {
      mp_set(c, high);
      goto LBL_ERR;
   } else {
      mp_set(c, low);
      goto LBL_ERR;
   }

LBL_ERR:
   mp_clear_multi(&bracket_low, &bracket_high, &bracket_mid,
                  &t, &bi_base, NULL);
   return err;
}
#endif



#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
