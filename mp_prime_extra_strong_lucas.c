#include "tommath_private.h"
#ifdef MP_PRIME_EXTRA_STRONG_LUCAS_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */



/*
 *  See file mp_prime_is_prime.c or the documentation in doc/bn.tex for the details
 */
#ifndef LTM_USE_ONLY_MR

/*
    Extra strong Lucas test with Baillie's parameters[1 method C].
    Returns true if it is an extra strong Lucas prime, false if it is composite.
    Assumes input is odd, non-square and larger than 5 (five);

    Computes an almost extra strong Lucas test first and uses Crandall and
    Pomerance's finding[2 p. 146 sec. 3.2.3]: U_m = D^-1 (2V_{m+1} - PV_m) to get
    an extra strong test (The "D" here is the upper case delta in the book).

    Implementation follows roughly section 3.6 in [2] with Grantham's definition
    of "extra strong Lucas test" in [3] (The definitions are not numbered. It is
    the one after theorem 2.3 on page 876 (page 4 in the PDF)).
    Jon Grantham's paper is heartily recommended, it gives some useful insights
    into this theme.


    [1] Baillie, "Extra strong Lucas pseudoprimes", OEIS A217719, https://oeis.org/A217719
    [2] Crandall and Pomerance, Prime Numbers: A Computational Perspective, 2nd ed. Springer, 2005
    [3] Grantham, "Frobenius Pseudoprimes", Mathematics of Computation 70(234), March 2000, pp. 873-891.
         https://www.ams.org/journals/mcom/2001-70-234/S0025-5718-00-01197-2/S0025-5718-00-01197-2.pdf
*/
mp_err mp_prime_extra_strong_lucas(const mp_int *N, bool *result)
{
   mp_int Dz, s, Vk, Vk1, tmp1, Nm2;
   uint32_t D, P;
   int32_t j = 0, r, nbits;
   int i;
   mp_err err = MP_OKAY;

   *result = false;

   if ((err = mp_init_multi(&Dz, &s, &Vk, &Vk1, &tmp1, &Nm2, NULL)) != MP_OKAY)      goto LTM_ERR;

   /* Limit for the loop computing D such that P^2 fits into a 32 bit integer */
#ifndef MP_ITER_LIMIT_ESLPSPT
#define MP_ITER_LIMIT_ESLPSPT 32768
#endif

   /* Increment for P is one here; it is(was?) two in Pari/GP */
   for (P = 3; P <= MP_ITER_LIMIT_ESLPSPT; P++) {
      D = (P * P) - 4;
      mp_set_u32(&Dz, D);
      if ((err = mp_kronecker(&Dz, N, &j)) != MP_OKAY)                               goto LTM_ERR;
      if (j == -1) {
         break;
      }
      if (j == 0) {
         /* Here both D and N  have the prime factor P+2 */
         if (mp_cmp_d(N, P + 2) == MP_EQ) {
            *result = true;
            goto LTM_ERR;
         } else {
            goto LTM_ERR;
         }
      }
      /*
         This is said to be extremely unlikely. The author would like to
         get a little note with the number that caused it if possible.
       */
      if (P == MP_ITER_LIMIT_ESLPSPT) {
         err = MP_ITER;
         goto LTM_ERR;
      }
   }
   /* s = (N - jacobi(D, N)) / 2^r and jacobi(D, N) = -1 at this point */
   if ((err = mp_add_d(N, 1u, &s)) != MP_OKAY)                                       goto LTM_ERR;
   r = mp_cnt_lsb(&s);
   if ((err = mp_div_2d(&s, r, &s, NULL)) != MP_OKAY)                                goto LTM_ERR;
   if ((err = mp_sub_d(N, 2u, &Nm2)) != MP_OKAY)                                     goto LTM_ERR;
   mp_set(&Vk, 2u);
   mp_set_u32(&Vk1, P);
   nbits = mp_count_bits(&s);
   /* Compute Lucas sequence of V */
   for (i = nbits; i >= 0; i--) {
      if ((err = mp_mul(&Vk, &Vk1, &tmp1)) != MP_OKAY)                               goto LTM_ERR;
      if ((err = mp_add(&tmp1, N, &tmp1)) != MP_OKAY)                                goto LTM_ERR;
      if ((err = mp_sub_d(&tmp1, P, &tmp1)) != MP_OKAY)                              goto LTM_ERR;
      if ((err = mp_mod(&tmp1, N, &tmp1)) != MP_OKAY)                                goto LTM_ERR;
      if (s_mp_get_bit(&s, i) == 1) {
         if ((err = mp_copy(&tmp1, &Vk)) != MP_OKAY)                                 goto LTM_ERR;
         if ((err = mp_sqr(&Vk1, &Vk1)) != MP_OKAY)                                  goto LTM_ERR;
         if ((err = mp_add(&Vk1, &Nm2, &Vk1)) != MP_OKAY)                            goto LTM_ERR;
         if ((err = mp_mod(&Vk1, N, &Vk1)) != MP_OKAY)                               goto LTM_ERR;
      } else {
         if ((err = mp_copy(&tmp1, &Vk1)) != MP_OKAY)                                goto LTM_ERR;
         if ((err = mp_sqr(&Vk, &Vk)) != MP_OKAY)                                    goto LTM_ERR;
         if ((err = mp_add(&Vk, &Nm2, &Vk)) != MP_OKAY)                              goto LTM_ERR;
         if ((err = mp_mod(&Vk, N, &Vk)) != MP_OKAY)                                 goto LTM_ERR;
      }
   }
   /* Check Vk = \pm 2 mod N */
   if ((mp_cmp_d(&Vk, 2u) == MP_EQ) || (mp_cmp(&Vk, &Nm2) == MP_EQ)) {
      /* Compute Uk as described in [2] and check for Uk == 0 */
      if ((err = mp_mul_d(&Vk, P, &tmp1)) != MP_OKAY)                                goto LTM_ERR;
      if ((err = mp_mul_2(&Vk1, &Vk1)) != MP_OKAY)                                   goto LTM_ERR;
      if ((err = mp_sub(&tmp1, &Vk1, &tmp1)) != MP_OKAY)                             goto LTM_ERR;
      tmp1.sign = MP_ZPOS;
      if ((err = mp_mod(&tmp1, N, &Vk1)) != MP_OKAY)                                 goto LTM_ERR;
      if (mp_iszero(&Vk1)) {
         *result = true;
         goto LTM_ERR;
      }
   }
   /* Check for V_{2^t k} = 0 mod N for some t 0 <= t < r - 1  ([3])*/
   for (i = 0; i < (r - 1); i++) {
      if (mp_iszero(&Vk)) {
         *result = true;
         goto LTM_ERR;
      }
      /* Loop: V(2k) = V(k)^2 - 2 and four minus two is two again */
      if (mp_cmp_d(&Vk, 2u) == MP_EQ) {
         goto LTM_ERR;
      }
      if ((err = mp_sqr(&Vk, &Vk)) != MP_OKAY)                                       goto LTM_ERR;
      if ((err = mp_sub_d(&Vk, 2u, &Vk)) != MP_OKAY)                                 goto LTM_ERR;
      if ((err = mp_mod(&Vk, N, &Vk)) != MP_OKAY)                                    goto LTM_ERR;
   }

LTM_ERR:
   mp_clear_multi(&Dz, &s, &Vk, &Vk1, &tmp1, &Nm2, NULL);
   return err;
}
#endif
#endif

