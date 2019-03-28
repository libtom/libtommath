#include "tommath_private.h"
#ifdef BN_MP_PRIME_IS_PRIME_DETERMINISTIC_C
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
/* Deterministic primality test. Assumes the generalized Riemann hypothesis
   and is slow, very slow for large primes.  */
int mp_prime_is_prime_deterministic(const mp_int *z, int *result)
{
   int ilog2, i;
   mp_int b;
   LTM_SIEVE_UINT limit;
   int e = MP_OKAY;

   const mp_digit small_bases[] = {
      2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43
   };
   /* First odd number for which the set of bases above fail */
   const unsigned char first_odd[] = {
      0x02, 0xbe, 0x69, 0x51, 0xad, 0xc5,
      0xb2, 0x24, 0x10, 0xa5, 0xfd
   };

   
   *result = MP_NO;

   if (IS_ZERO(z) || (z->sign == MP_NEG)) {
      return MP_VAL;
   }

   if ((e = mp_prime_is_prime(z, 8, result)) != MP_OKAY) {
      *result = MP_NO;
      return e;
   }
   if (*result == MP_NO) {
      return e;
   }

   ilog2 = mp_count_bits(z);
   /*
      Sorenson, Jonathan; Webster, Jonathan (2015). "Strong Pseudoprimes to Twelve Prime Bases".
      Mathematics of Computation. 86 (304): 985–1003. arXiv:1509.00864. doi:10.1090/mcom/3134.
      https://arxiv.org/abs/1509.00864

      mp_count_bits(3317044064679887385961981) = 82
             2^81 = 2417851639229258349412352
       Last prime < 3317044064679887385961981 is 3317044064679887385961813
             2^82 = 4835703278458516698824704
    */
   if (ilog2 < 83) {
      if ((e = mp_init(&b)) != MP_OKAY) {
         goto LTM_ERR_2;
      }
      if ((e = mp_read_unsigned_bin(&b, first_odd, 82)) != MP_OKAY) {
         goto LTM_ERR;
      }
      if (mp_cmp(z,&b) == MP_LT) {
         for (i= 0; i < (int)(sizeof(small_bases)/sizeof(small_bases[0])); i++) {
            mp_set(&b, small_bases[i]);
            if (mp_cmp_d(z, small_bases[i]+2) == MP_GT) {
               if ((e = mp_prime_miller_rabin(z, &b, result)) != MP_OKAY) {
                  goto LTM_ERR;
               }
               if (*result == MP_NO) {
                  goto LTM_END;
               }
            } else {
               break;
            }
         }
         goto LTM_END;
      }
   }
   limit = (LTM_SIEVE_UINT)(((ilog2 * 70) / 100) + 1);
   limit *= limit;
   limit *= 2;
   if ((e = mp_miller_bach(z, limit, result)) != MP_OKAY) {
      *result = MP_NO;
      goto LTM_ERR_2;
   }

   return e;
LTM_END:
   mp_clear(&b);
   return e;
LTM_ERR:
   mp_clear(&b);
LTM_ERR_2:
   return e;
}
#endif



#endif
/* ref:         \$Format:\%D$ */
/* git commit:  \$Format:\%H$ */
/* commit time: \$Format:\%ai$ */
