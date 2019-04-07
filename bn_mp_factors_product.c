#include "tommath_private.h"
#ifdef BN_MP_FACTORS_PRODUCT_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* Compute the product of the elements of "factors" */
static int s_product_lowlevel(const mp_factors *factors, int start, int n, mp_int *result)
{
   int i, first_half, second_half;
   mp_int tmp;
   int e = MP_OKAY;

   if (n == 0) {
      mp_set(result, 1uL);
      return MP_OKAY;
   }
   /* Do the rest linearily. Faster for primorials at least,  but YMMV */
   if (n <= 64) {
      mp_set_long(result,1uL);
      for (i = 0; i < n; i++) {
         if ((e = mp_mul(result, &(factors->factors[i+start]), result)) != MP_OKAY) {
            return e;
         }
      }
      return MP_OKAY;
   }

   first_half = n / 2;
   second_half = n - first_half;
   if ((e = s_product_lowlevel(factors, start, second_half, result)) != MP_OKAY) {
      return e;
   }
   if ((e = mp_init(&tmp)) != MP_OKAY) {
      return e;
   }
   if ((e = s_product_lowlevel(factors, start+second_half, first_half, &tmp)) != MP_OKAY) {
      goto LTM_ERR;
   }
   if ((e = mp_mul(result, &tmp, result)) != MP_OKAY) {
      goto LTM_ERR;
   }

LTM_ERR:
   mp_clear(&tmp);
   return e;
}

int mp_factors_product(const mp_factors *factors, mp_int *p)
{
   int e, r;
   r = factors->length;
   if (r == 0) {
      return MP_VAL;
   }
   if (r == 1) {
      return mp_copy(&(factors->factors[0]), p);
   }
   if ((e = s_product_lowlevel(factors, 0, r, p)) != MP_OKAY) {
      return e;
   }
   return MP_OKAY;
}
#endif
