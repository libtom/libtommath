#include "tommath_private.h"
#ifdef MP_PRIME_IS_PRIME_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* portable integer log of two with small footprint */
static unsigned int s_floor_ilog2(int value)
{
   unsigned int r = 0;
   while ((value >>= 1) != 0) {
      r++;
   }
   return r;
}

mp_err mp_prime_is_prime(const mp_int *a, int t, bool *result)
{
   mp_int  b;
   int     ix, bits;
   bool    res;
   mp_err  err = MP_OKAY;

#ifdef LTM_USE_ONLY_MR

   /* 2, 7, 61 found by Gerhard Jaeschke 1993 (correctness confirmed by author) */
   /*
      Because we already have 2 and 3 at this point all four bases are good up to

         33717240721: 129841 259681                     34.9728   y

      This is the single SPRP up to 51706044253 (~2^35.5896). Next are

         163204128181: 285661 571321                    37.2479   y
         501086407781: 288989 1733929                   38.8663   y
            ... 23588 others skipped ...
         18441334942415579101: 876577981 21037871521    63.9996   n
         18444384017352327673: 1920644893 9603224461    63.9998   n

      Adding base 5 brings us to

         10087771603687: 1588063 6352249                43.1977   y

      The record for five bases is at 7999252175582851 for now, about 2^52.8288 bit large
      but hase bases larger than 32 bit and none of the known 5-base records has bases
      smaller than 28 bit.

      Adding base 63803 (already > 2^15) brings us to

        849491953715047: 14573023 58292089              49.5936   y

      Record for 6 bases is at 585226005592931977 ~2^59.0217 but one base is over 60 bit
      large.

    */
   mp_digit bases32[] = {5u, 7u, 61u};
   /* 2, 325, 9375, 28178, 450775, 9780504, 1795265022 found by Jim Sinclair 2011
     (correctness confirmed by author)

     An alternative would be 2, 3, 673, 325, 9375, 28178, 450775, 9780504 which has
     8 bases but all bases are smaller than 28 bit.
     The first 2-SPRP > 2^64 is 18446744073709551617 which is not a 3-SPRP
    */
   mp_word bases64[] = {325ull, 9375ull, 28178ull, 450775ull, 9780504ull, 1795265022ull};
#if (MP_DIGIT_BIT < 31)
   mp_digit rem;
#endif
#endif

   /* default to no */
   *result = false;

   /* Some shortcuts */
   /* N > 3 */
   if (a->used == 1) {
      if ((a->dp[0] == 0u) || (a->dp[0] == 1u)) {
         *result = false;
         return MP_OKAY;
      }
      if (a->dp[0] == 2u) {
         *result = true;
         return MP_OKAY;
      }
   }

   /* N must be odd */
   if (mp_iseven(a)) {
      return MP_OKAY;
   }
   /* N is not a perfect square: floor(sqrt(N))^2 != N */
   if ((err = mp_is_square(a, &res)) != MP_OKAY) {
      return err;
   }
   if (res) {
      return MP_OKAY;
   }

   /* is the input equal to one of the primes in the table? */
   for (ix = 0; ix < MP_PRIME_TAB_SIZE; ix++) {
      if (mp_cmp_d(a, s_mp_prime_tab[ix]) == MP_EQ) {
         *result = true;
         return MP_OKAY;
      }
   }
   /* first perform trial division */
   if ((err = s_mp_prime_is_divisible(a, &res)) != MP_OKAY) {
      return err;
   }
   /* return if it was trivially divisible */
   if (res) {
      return MP_OKAY;
   }
   /* floor(log_2(a)) */
   bits = mp_count_bits(a) - 1;

   /* If the whole prime table up to p = 1619 has been tested, than  all
      numbers below 1621^2 = 2,627,641 are prime now. log_2(1621^2) ~ 21.33 */
   if (bits < 21) {
      *result = true;
      return MP_OKAY;
   }

   /*
       Run the Miller-Rabin test with base 2 for the BPSW test.
    */
   if ((err = mp_init_set(&b, 2uL)) != MP_OKAY) {
      return err;
   }

   if ((err = mp_prime_miller_rabin(a, &b, &res)) != MP_OKAY) {
      goto LBL_B;
   }
   if (!res) {
      goto LBL_B;
   }
   /* If the whole prime table up to p = 1619 and the Miller-Rabin tests to base two
      has been applied, than all numbers below 4,469,471 (~2^{22.1}) are prime now.
      With 1659 SPSPs < 2^32 left */
#if ((defined S_MP_PRIME_IS_DIVISIBLE_C) && (MP_PRIME_TAB_SIZE >= 256))
   if (bits < 22) {
      *result = true;
      goto LBL_B;
   }
#endif
   /*
      Rumours have it that Mathematica does a second M-R test with base 3.
      Other rumours have it that their strong L-S test is slightly different.
      It does not hurt, though, beside a bit of extra runtime.
   */
   b.dp[0]++;
   if ((err = mp_prime_miller_rabin(a, &b, &res)) != MP_OKAY) {
      goto LBL_B;
   }
   if (!res) {
      goto LBL_B;
   }

   /* If the whole prime table up to p = 1619 and the Miller-Rabin tests to bases
      two and three have been applied, than all numbers below 11,541,307 (~2^{23.5}) are prime now.
      With 89 SPRPs < 2^32 left */
#if ((defined S_MP_PRIME_IS_DIVISIBLE_C) && (MP_PRIME_TAB_SIZE >= 256))
   if (bits < 23) {
      *result = true;
      goto LBL_B;
   }
#endif
   /*
    * Both, the Frobenius-Underwood test and the the extra strong Lucas test are quite
    * slow so if speed is an issue, define LTM_USE_ONLY_MR to use M-R tests with
    * bases 2, 3 and t random bases.
    */
#ifndef LTM_USE_ONLY_MR
   if (t >= 0) {
      if ((err = mp_prime_extra_strong_lucas(a, &res)) != MP_OKAY) {
         goto LBL_B;
      }
      if (!res) {
         goto LBL_B;
      }
      /*
         The Frobenius-Underwood pseudoprimes are sufficiently different from the
         Extra Strong Lucas pseudoprimes with the parameters used in this library
         to offer it as an additionally test (but it nearly doubles the runtime).
       */
#ifdef LTM_USE_FROBENIUS_TEST
      err = mp_prime_frobenius_underwood(a, &res);
      if ((err != MP_OKAY) && (err != MP_ITER)) {
         goto LBL_B;
      }
      if (!res) {
         goto LBL_B;
      }
#endif
   }
#endif


   if (t == 0) {
#ifndef LTM_USE_ONLY_MR
      /*
          The BPSW version as used here is deterministic below 2^64
          (correctness confirmed by author)
       */
      if (bits < 64) {
         *result = true;
         goto LBL_B;
      }
#endif
      /* run at least one Miller-Rabin test with a random base if n > 2^64 */
      t = 1;
   }

   /*
      Only recommended if the input range is known to be < 3317044064679887385961981

      It uses the bases necessary for a deterministic M-R test if the input is
      smaller than  3317044064679887385961981
      The caller has to check the maximum size.
   */
   if (t < 0) {
      int p_max = 0;
#ifndef LTM_USE_ONLY_MR
      if (bits < 64) {
         /* Just complete the BPSW test */
         if ((err = mp_prime_extra_strong_lucas(a, &res)) != MP_OKAY) {
            goto LBL_B;
         }
         *result = res;
         goto LBL_B;
      }
#else
      /*
         Bases 2 and 3 are already done. Base 1459:
            1530787 = 2473 * 619          <- already out by trial division
            1518290707 = 19483 * 77929    <- trial division by 19483 (<2^15) or check for n == 1518290707 (~2^30.5)

         This holds for a while. Next SPRPs < 2^35 to check with {2, 3, 1459}:

                n          factors         log_2(n)    has a factor < 2^28
            6770862367:  41143 164569      32.6567             y
            15579919981: 88261 176521      33.859              y
            16149644101: 63541 254161      33.9108             y
            17849326081: 50497 353473      34.0556             y
            23510118061: 108421 216841     34.4526             y
            24988416967: 79039 316153      34.5405             y
            27031263841: 116257 232513     34.6539             y
            28448982721: 97381 292141      34.7276             y

       */
#if ((defined S_MP_PRIME_IS_DIVISIBLE_C) && (MP_PRIME_TAB_SIZE >= 256))
      if (bits < 32) {
#if (MP_DIGIT_BIT < 31)
         if ((err = mp_div_d(a, 19483u, NULL, &rem)) != MP_OKAY)        goto LBL_B;
         if (rem == 0u) {
            goto LBL_B;
         }
#else
         if (mp_cmp_d(a, 1518290707u) == MP_EQ) {
            goto LBL_B;
         }
#endif
         mp_set(&b, 1459u);
         if ((err = mp_prime_miller_rabin(a, &b, &res)) != MP_OKAY) {
            goto LBL_B;
         }
         *result = res;
         goto LBL_B;
      }
#endif
      if (bits < 43) {
         for (ix = 0; ix < 3; ix++) {
            mp_set(&b, bases32[ix]);
            if ((err = mp_prime_miller_rabin(a, &b, &res)) != MP_OKAY) {
               goto LBL_B;
            }
            if (!res) {
               goto LBL_B;
            }
         }
         *result = true;
         goto LBL_B;
      } else if ((bits >= 43) && (bits < 64)) {
         for (ix = 0; ix < 6; ix++) {
            mp_set_u32(&b, bases64[ix]);
            if ((err = mp_prime_miller_rabin(a, &b, &res)) != MP_OKAY) {
               goto LBL_B;
            }
            if (!res) {
               goto LBL_B;
            }
         }
         *result = true;
         goto LBL_B;
      }
#endif /* End of LTM_USE_ONLY_MR */
      /*
          Sorenson, Jonathan; Webster, Jonathan,  "Strong Pseudoprimes to Twelve Prime Bases". (2015) https://arxiv.org/abs/1509.00864
          Z. Zhang, "Finding strong pseudoprimes to several bases," Math. Comp., 70:234 (2001) 863--87
          Z. Zhang, "Finding C3-strong pseudoprimes," Math. Comp., 74:250 (2005) 1009--1024

          Not implemented per default here but can be switched on via preprocessor directive "LTM_USE_ZHANG":
          Zhang, Zhenxiang, "Two kinds of strong pseudoprimes up to 10^36," Math. Comp., 76:260 (2007) 2095--2107
          (10^36 > 2^119)
       */
      else if ((bits >= 64) && (bits <= 78)) {
         /* 0x437ae92817f9fc85b7e5 = 318665857834031151167461 */
         if ((err =   mp_read_radix(&b, "437ae92817f9fc85b7e5", 16)) != MP_OKAY) {
            goto LBL_B;
         }
         if (mp_cmp(a, &b) == MP_LT) {
            p_max = 12;
         } else {
            p_max = 13;
         }
      } else if ((bits >= 78) && (bits <= 82)) {
         /* 0x2be6951adc5b22410a5fd = 3317044064679887385961981 */
         if ((err = mp_read_radix(&b, "2be6951adc5b22410a5fd", 16)) != MP_OKAY) {
            goto LBL_B;
         }
         if (mp_cmp(a, &b) == MP_LT) {
            p_max = 13;
         }
         /* The verified deterministic part ends here */
#ifndef LTM_USE_ZHANG
         else {
            err = MP_VAL;
            goto LBL_B;
         }
#endif
      }
      /* The unverified deterministic part starts here */
#ifdef LTM_USE_ZHANG
      /*
          USE WITH CAUTION!

          These bounds have not yet been independently verified.
          But they are a good and relatively cheap method for sieving if an expensive full
          deterministic primetest follows. (The Miller-Rabin test has no false negatives).
       */
      /* 6003094289670105800312596501 = 54786377365501 * 109572754731001 ~2^92.2777 14 rounds */
      else if ((bits > 81) && (bits <= 92)) {
         p_max = 14;
      }
      /* 59276361075595573263446330101 = 172157429516701 * 344314859033401 ~2^95.5814 15 rounds */
      else if ((bits > 92) && (bits <= 95)) {
         p_max = 15;
      }
      /* 564132928021909221014087501701 = 531099297693901 * 1062198595387801  ~2^98.8319  16 rounds*/
      else if ((bits > 95) && (bits <= 98)) {
         p_max = 16;
      }
      /* 1543267864443420616877677640751301 = 27778299663977101 * 55556599327954201 ~2^110.2496 18 rounds */
      else if ((bits > 98) && (bits <= 111)) {
         if ((err = mp_read_radix(&b, "4c16c7697197146a6b8eb49518c5", 16)) != MP_OKAY) {
            goto LBL_B;
         }
         if (mp_cmp(a, &b) == MP_LT) {
            p_max = 18;
         } else {
            err = MP_VAL;
            goto LBL_B;
         }
      }
#endif
      else {
         err = MP_VAL;
         goto LBL_B;
      }

      /* we did bases 2 and 3  already, skip them */
      for (ix = 2; ix < p_max; ix++) {
         mp_set(&b, s_mp_prime_tab[ix]);
         if ((err = mp_prime_miller_rabin(a, &b, &res)) != MP_OKAY) {
            goto LBL_B;
         }
         if (!res) {
            goto LBL_B;
         }
      }
   }
   /*
       Do "t" M-R tests with random bases between 3 and "a".
       See Fips 186.4 p. 126ff
   */
   if (t > 0) {
      unsigned int mask;
      /*
       * The mp_digit's have a defined bit-size but the size of the
       * array a.dp is a simple 'int' and this library can not assume full
       * compliance to the current C-standard (ISO/IEC 9899:2011) because
       * it gets used for small embedded processors, too. Some of those MCUs
       * have compilers that one cannot call standard compliant by any means.
       * Hence the ugly type-fiddling in the following code.
       */
      bits = mp_count_bits(a);
      mask = (1u << s_floor_ilog2(bits)) - 1u;
      /*
         Assuming the General Rieman hypothesis (never thought to write that in a
         comment) the upper bound can be lowered to  2*(log a)^2.
         E. Bach, "Explicit bounds for primality testing and related problems,"
         Math. Comp. 55 (1990), 355-380.

            bits = (bits/10) * 7;
            len = 2 * (bits * bits);

         E.g.: a number of size 2^2048 would be reduced to the upper limit

            floor(2048/10)*7 = 1428
            2 * 1428^2       = 4078368

         (would have been ~4030331.9962 with floats and natural log instead)
         That number is smaller than 2^28, the default bit-size of mp_digit.
      */

      /*
        How many tests, you might ask? Dana Jacobsen of Math::Prime::Util fame
        does exactly 1. In words: one. Look at the end of _GMP_is_prime() in
        Math-Prime-Util-GMP-0.50/primality.c if you do not believe it.

        The function mp_rand() goes to some length to use a cryptographically
        good PRNG. That also means that the chance to always get the same base
        in the loop is non-zero, although very low.
        If the BPSW test and/or the additional Frobenious test have been
        performed instead of just the Miller-Rabin test with the bases 2 and 3,
        a single extra test should suffice, so such a very unlikely event
        will not do much harm.

        To preemptively answer the dangling question: no, a witness does not
        need to be prime.
      */
      for (ix = 0; ix < t; ix++) {
         unsigned int fips_rand;
         int len;
         /* mp_rand() guarantees the first digit to be non-zero */
         if ((err = mp_rand(&b, 1)) != MP_OKAY) {
            goto LBL_B;
         }
         /*
          * Reduce digit before casting because mp_digit might be bigger than
          * an unsigned int and "mask" on the other side is most probably not.
          */
         fips_rand = (unsigned int)(b.dp[0] & (mp_digit) mask);
         if (fips_rand > (unsigned int)(INT_MAX - MP_DIGIT_BIT)) {
            len = INT_MAX / MP_DIGIT_BIT;
         } else {
            len = (((int)fips_rand + MP_DIGIT_BIT) / MP_DIGIT_BIT);
         }
         /*  Unlikely. */
         if (len < 0) {
            ix--;
            continue;
         }
         if ((err = mp_rand(&b, len)) != MP_OKAY) {
            goto LBL_B;
         }
         /*
          * That number might got too big and the witness has to be
          * smaller than "a"
          */
         len = mp_count_bits(&b);
         if (len >= bits) {
            len = (len - bits) + 1;
            if ((err = mp_div_2d(&b, len, &b, NULL)) != MP_OKAY) {
               goto LBL_B;
            }
         }
         /* Although the chance for b <= 3 is miniscule, try again. */
         if (mp_cmp_d(&b, 3uL) != MP_GT) {
            ix--;
            continue;
         }
         if ((err = mp_prime_miller_rabin(a, &b, &res)) != MP_OKAY) {
            goto LBL_B;
         }
         if (!res) {
            goto LBL_B;
         }
      }
   }

   /* passed the test */
   *result = true;
LBL_B:
   mp_clear(&b);
   return err;
}

#endif
