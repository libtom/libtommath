#include "tommath_private.h"
#ifdef BN_MP_PRIME_NEXT_PRIME_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* finds the next prime after the number "a" using "t" trials
 * of Miller-Rabin.
 *
 * bbs_style = 1 means the prime must be congruent to 3 mod 4
 */
mp_err mp_prime_next_prime(mp_int *a, int t, int bbs_style)
{
   int      x, y;
   mp_err  err;
   mp_bool res = MP_NO;
   mp_digit res_tab[MP_PRIME_SIZE], step, kstep;
   mp_int   b;

   /* force positive */
   a->sign = MP_ZPOS;

   /* simple algo if a is less than the largest prime in the table */
   if (mp_cmp_d(a, ltm_prime_tab[MP_PRIME_SIZE-1]) == MP_LT) {
      /* find which prime it is bigger than */
      for (x = MP_PRIME_SIZE - 2; x >= 0; x--) {
         if (mp_cmp_d(a, ltm_prime_tab[x]) != MP_LT) {
            if (bbs_style == 1) {
               /* ok we found a prime smaller or
                * equal [so the next is larger]
                *
                * however, the prime must be
                * congruent to 3 mod 4
                */
               if ((ltm_prime_tab[x + 1] & 3u) != 3u) {
                  /* scan upwards for a prime congruent to 3 mod 4 */
                  for (y = x + 1; y < MP_PRIME_SIZE; y++) {
                     if ((ltm_prime_tab[y] & 3u) == 3u) {
                        mp_set_u(a, (uint32_t)ltm_prime_tab[y]);
                        return MP_OKAY;
                     }
                  }
               }
            } else {
               mp_set_u(a, (uint32_t)ltm_prime_tab[x + 1]);
               return MP_OKAY;
            }
         }
      }
      /* at this point a maybe 1 */
      if (mp_cmp_d(a, 1uL) == MP_EQ) {
         mp_set_u(a, 2u);
         return MP_OKAY;
      }
      /* fall through to the sieve */
   }

   /* generate a prime congruent to 3 mod 4 or 1/3 mod 4? */
   if (bbs_style == 1) {
      kstep   = 4;
   } else {
      kstep   = 2;
   }

   /* at this point we will use a combination of a sieve and Miller-Rabin */

   if (bbs_style == 1) {
      /* if a mod 4 != 3 subtract the correct value to make it so */
      if ((a->dp[0] & 3u) != 3u) {
         if ((err = mp_sub_d(a, (a->dp[0] & 3u) + 1u, a)) != MP_OKAY) {
            return err;
         };
      }
   } else {
      if (MP_IS_EVEN(a)) {
         /* force odd */
         if ((err = mp_sub_d(a, 1uL, a)) != MP_OKAY) {
            return err;
         }
      }
   }

   /* generate the restable */
   for (x = 1; x < MP_PRIME_SIZE; x++) {
      if ((err = mp_mod_d(a, ltm_prime_tab[x], res_tab + x)) != MP_OKAY) {
         return err;
      }
   }

   /* init temp used for Miller-Rabin Testing */
   if ((err = mp_init(&b)) != MP_OKAY) {
      return err;
   }

   for (;;) {
      /* skip to the next non-trivially divisible candidate */
      step = 0;
      do {
         /* y == 1 if any residue was zero [e.g. cannot be prime] */
         y     =  0;

         /* increase step to next candidate */
         step += kstep;

         /* compute the new residue without using division */
         for (x = 1; x < MP_PRIME_SIZE; x++) {
            /* add the step to each residue */
            res_tab[x] += kstep;

            /* subtract the modulus [instead of using division] */
            if (res_tab[x] >= ltm_prime_tab[x]) {
               res_tab[x]  -= ltm_prime_tab[x];
            }

            /* set flag if zero */
            if (res_tab[x] == 0u) {
               y = 1;
            }
         }
      } while ((y == 1) && (step < (((mp_digit)1 << MP_DIGIT_BIT) - kstep)));

      /* add the step */
      if ((err = mp_add_d(a, step, a)) != MP_OKAY) {
         goto LBL_ERR;
      }

      /* if didn't pass sieve and step == MP_MAX then skip test */
      if ((y == 1) && (step >= (((mp_digit)1 << MP_DIGIT_BIT) - kstep))) {
         continue;
      }

      if ((err = mp_prime_is_prime(a, t, &res)) != MP_OKAY) {
         goto LBL_ERR;
      }
      if (res == MP_YES) {
         break;
      }
   }

   err = MP_OKAY;
LBL_ERR:
   mp_clear(&b);
   return err;
}

#endif
