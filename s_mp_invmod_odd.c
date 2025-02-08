#include "tommath_private.h"
#ifdef S_MP_INVMOD_ODD_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/* computes the modular inverse via binary extended euclidean algorithm,
 * that is c = 1/a mod b
 *
 * Based on slow invmod except this is optimized for the case where b is
 * odd as per HAC Note 14.64 on pp. 610
 */
mp_err s_mp_invmod_odd(const mp_int *a, const mp_int *b, mp_int *c)
{
   mp_int  x, y, u, v, B, D;
   mp_err  err = MP_OKAY;

   /* 2. [modified] b must be odd   */
   if (mp_iseven(b)) {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR);
   }

   /* init all our temps */
   if ((err = mp_init_multi(&x, &y, &u, &v, &B, &D, NULL)) != MP_OKAY)   MP_TRACE_ERROR(err, LTM_ERR);

   /* x == modulus, y == value to invert */
   if ((err = mp_copy(b, &x)) != MP_OKAY)                                MP_TRACE_ERROR(err, LTM_ERR_1);

   /* y needs to be positive but the remainder d of mp_div(a,b,c,d) might be negative */
   if ((err = mp_mod(a, b, &y)) != MP_OKAY)                              MP_TRACE_ERROR(err, LTM_ERR_1);

   /* if one of x,y is zero return an error! */
   if (mp_iszero(&x) || mp_iszero(&y)) {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR_1);
   }

   /* 3. u=x, v=y, A=1, B=0, C=0,D=1 */
   if ((err = mp_copy(&x, &u)) != MP_OKAY)                               MP_TRACE_ERROR(err, LTM_ERR_1);
   if ((err = mp_copy(&y, &v)) != MP_OKAY)                               MP_TRACE_ERROR(err, LTM_ERR_1);
   mp_set(&D, 1uL);

   do {
      /* 4.  while u is even do */
      while (mp_iseven(&u)) {
         /* 4.1 u = u/2 */
         if ((err = mp_div_2(&u, &u)) != MP_OKAY)                        MP_TRACE_ERROR(err, LTM_ERR_1);

         /* 4.2 if B is odd then */
         if (mp_isodd(&B)) {
            if ((err = mp_sub(&B, &x, &B)) != MP_OKAY)                   MP_TRACE_ERROR(err, LTM_ERR_1);
         }
         /* B = B/2 */
         if ((err = mp_div_2(&B, &B)) != MP_OKAY)                        MP_TRACE_ERROR(err, LTM_ERR_1);
      }

      /* 5.  while v is even do */
      while (mp_iseven(&v)) {
         /* 5.1 v = v/2 */
         if ((err = mp_div_2(&v, &v)) != MP_OKAY)                        MP_TRACE_ERROR(err, LTM_ERR_1);

         /* 5.2 if D is odd then */
         if (mp_isodd(&D)) {
            /* D = (D-x)/2 */
            if ((err = mp_sub(&D, &x, &D)) != MP_OKAY)                   MP_TRACE_ERROR(err, LTM_ERR_1);
         }
         /* D = D/2 */
         if ((err = mp_div_2(&D, &D)) != MP_OKAY)                        MP_TRACE_ERROR(err, LTM_ERR_1);
      }

      /* 6.  if u >= v then */
      if (mp_cmp(&u, &v) != MP_LT) {
         /* u = u - v, B = B - D */
         if ((err = mp_sub(&u, &v, &u)) != MP_OKAY)                      MP_TRACE_ERROR(err, LTM_ERR_1);

         if ((err = mp_sub(&B, &D, &B)) != MP_OKAY)                      MP_TRACE_ERROR(err, LTM_ERR_1);
      } else {
         /* v - v - u, D = D - B */
         if ((err = mp_sub(&v, &u, &v)) != MP_OKAY)                      MP_TRACE_ERROR(err, LTM_ERR_1);

         if ((err = mp_sub(&D, &B, &D)) != MP_OKAY)                      MP_TRACE_ERROR(err, LTM_ERR_1);
      }

      /* if not zero goto step 4 */
   } while (!mp_iszero(&u));

   /* now a = C, b = D, gcd == g*v */

   /* if v != 1 then there is no inverse */
   if (mp_cmp_d(&v, 1uL) != MP_EQ) {
      err = MP_VAL;
      MP_TRACE_ERROR(err, LTM_ERR_1);
   }

   /* b is now the inverse */
   while (mp_isneg(&D)) {
      if ((err = mp_add(&D, b, &D)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR_1);
   }

   /* too big */
   while (mp_cmp_mag(&D, b) != MP_LT) {
      if ((err = mp_sub(&D, b, &D)) != MP_OKAY)                          MP_TRACE_ERROR(err, LTM_ERR_1);
   }

   mp_exch(&D, c);

LTM_ERR_1:
   mp_clear_multi(&x, &y, &u, &v, &B, &D, NULL);
LTM_ERR:
   return err;
}
#endif
